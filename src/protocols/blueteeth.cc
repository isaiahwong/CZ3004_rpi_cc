/**
 * @file blueteeth.cc
 * @author isaiahwong
 * @brief
 * @version 0.1
 * @date 2022-01-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "blueteeth.h"

#include <errno.h>
#include <fcntl.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

#include "action.h"

Blueteeth::Blueteeth() {
    this->name = "blueteeth";
    this->channel = 1;
    this->clientAddr = new sockaddr_rc();
    this->localAddr = new sockaddr_rc();
    this->commands = BlockingQueueAction();
    this->statuses = BlockingQueueRes();
    this->init();
}

Blueteeth::Blueteeth(int channel, std::string name) {
    this->name = name;
    this->channel = channel;

    // RFCOMM socket address
    this->clientAddr = new sockaddr_rc();
    this->localAddr = new sockaddr_rc();
    this->init();
}

Blueteeth::Blueteeth(int channel, std::string name, int instructionDelay) {
    this->name = name;
    this->channel = channel;
    this->instructionDelay = instructionDelay;
    // RFCOMM socket address
    this->clientAddr = new sockaddr_rc();
    this->localAddr = new sockaddr_rc();
    this->init();
}

/**
 * @brief Invokes function to run on thread
 *
 */
void Blueteeth::run() {
    // Create connection Read Thread
    std::thread *connThread = new std::thread(
        static_cast<void (*)(void *c)>(Blueteeth::onConnectionRead), this);
    std::thread *receiveActionsThread = new std::thread(
        static_cast<void (*)(void *c)>(Blueteeth::onExecuteActions), this);

    // Push to protocol subthreads
    subThreads.push_back(UniqueThreadPtr(connThread));
    subThreads.push_back(UniqueThreadPtr(receiveActionsThread));
}

void Blueteeth::init() {
    std::string fn = this->genFnName(this->name, "init");
    int connections = 1;

    (*localAddr).rc_family = AF_BLUETOOTH;
    // Allows any bluetooth address to be used
    (*localAddr).rc_bdaddr = LOCAL;
    // Sets RFComm channel
    (*localAddr).rc_channel = (uint8_t)channel;

    // Creates RFComm BT Socket
    int &bluetoothSocket = this->bluetoothSocket;
    bluetoothSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (bluetoothSocket == -1)
        throw this->genError(fn, "Error encountered creating BT socket");

    // Binds socket to first available local bluetooth adapter
    int status =
        bind(bluetoothSocket, (struct sockaddr *)localAddr, sizeof(localAddr));

    if (status == -1) throw this->genError(fn, "Bluetooth bind failed", errno);

    status = listen(bluetoothSocket, connections);
    if (status == -1)
        throw this->genError(fn, "Bluetooth listen failed", errno);
}

void Blueteeth::onConnectionRead(void *b) {
    static_cast<Blueteeth *>(b)->onConnectionRead();
}

void Blueteeth::onConnectionRead() {
    while (true) {
        connect();
        this->readClient();
    }
}

void Blueteeth::onExecuteActions(void *b) {
    static_cast<Blueteeth *>(b)->onExecuteActions();
}

/**
 * @brief Waits for commands queue
 *
 */
void Blueteeth::onExecuteActions() {
    Action a;
    Response statusResponse;

    // TODO return stop

    int retries = 0, MAX_RETRIES = 3;
    bool restore = true;
    while (true) {
    queue:
        commands.wait_dequeue(a);
        retries = 0;
        restore = true;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(instructionDelay));
        while (true) {
            if (a.type.compare(Action::TYPE_MOVE) == 0) {
                this->publish(Blueteeth::BT_MOVEMENT, a);
            } else if (a.type.compare(Action::TYPE_CAPTURE) == 0) {
                this->publish(Blueteeth::BT_CAMERA_CAPTURE, a);
            } else {
                printRed("Unknown command in series");
                goto queue;
            }

            bool didReceive = statuses.wait_dequeue_timed(
                statusResponse, std::chrono::seconds(6));

            // Break queue if successful
            if (didReceive && statusResponse.status == 1) break;

            // Print notification for debug
            printRed("No Response");
            // Retry only for Image Rec
            if (a.type.compare(Action::TYPE_CAPTURE) != 0) break;

            if (retries++ < MAX_RETRIES) continue;
            printRed("Max retries, skipping command");
            statusResponse.status = 0;
            statusResponse.result = "-1";
            // Execute camera strategy if pass commands are not cached
            if (cached.size_approx() == 0) {
                cameraStrategy();
                restore = false;
            }
            break;
            // goto queue;
        }

        try {
            // Send response to android to notify success
            // Echo back the coordinate given
            Response response(a.type, statusResponse.result,
                              statusResponse.name, statusResponse.status,
                              a.coordinate, a.prev_coordinate,
                              statusResponse.distance);

            json j;
            response.to_json(j);
            std::string payload = j.dump();
            int size = payload.size();

            // Write back to client
            write(client, payload.c_str(), size);
            print("Command Executed\n\n");
        } catch (const std::exception &exc) {
            printRed("onExecuteActions Exeception: ");
            std::cout << exc.what() << std::endl;
            continue;
        }

        if (!restore) continue;

        // Restore commands queue on next pass if set to false
        if (commands.size_approx() == 0 && cached.size_approx() > 0) {
            printRed("Restoring commands");
            Action a;
            while (cached.try_dequeue(a)) commands.enqueue(a);
        }
    }
}

void Blueteeth::cameraStrategy() {
    // If cached is not empty, we do not clear it
    if (cached.size_approx() > 0) return;
    // If commands is empty, we don't cache it
    if (commands.size_approx() == 0) return;
    printRed("Running camera strategy");
    // empty commands
    Action a;
    while (commands.try_dequeue(a)) cached.enqueue(a);
    printRed("Done copy commands -> cache");
}

/**
 * @brief Blocking. Listens to bluetooth client
 *
 * @param bluetoothSocket
 */
void Blueteeth::connect() {
    // Socket length options
    socklen_t clientOptLen = sizeof(clientAddr);
    fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
               "Bluetooth listening for connections.\n");

    client = accept(this->bluetoothSocket, (struct sockaddr *)this->clientAddr,
                    &clientOptLen);

    if (client == -1) {
        fmt::print("Bluetooth connect failed\n");
        // TODO: Retry
        return;
    }

    char buf[1024] = {0};

    // Converts sockaddr_rc to string
    // ba2str(&(*localAddr).rc_bdaddr, buf);
    fmt::print(fmt::emphasis::bold | fg(fmt::color::lime_green),
               "Accepted connection from {} \n", buf);
    // Clear buffer
    memset(buf, 0, sizeof(buf));
}

/**
 * @brief disconnect function
 *
 */
void Blueteeth::disconnect() {
    if (!close(client) && !close(bluetoothSocket))
        fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                   "BT closed\n");
}

void Blueteeth::onResponse(void *b, Response *response) {
    static_cast<Blueteeth *>(b)->onResponse(response);
}

void Blueteeth::onResponse(Response *response) {
    if (response == nullptr) return;

    // Notify status of message
    statuses.enqueue(*response);
}

void Blueteeth::onAction(Action &action) {
    resetCommands();
    commands.enqueue(action);
}

void Blueteeth::onSeriesActions(Action &action) {
    if (action.data.size() < 1) return;
    resetCommands();
    // Enqueue
    for (Action a : action.data) {
        commands.enqueue(a);
    }
}

void Blueteeth::resetCommands() {
    Action a;

    while (commands.try_dequeue(a))
        ;
    printRed("Commands cleared");
}

void Blueteeth::resetCache() {
    Action a;
    while (cached.try_dequeue(a))
        ;
    printRed("Cache cleared");
}

/**
 * @brief Publishes incoming bluetooth connections
 *
 */
void Blueteeth::readClient() {
    // Wait for bluetooth client to accept
    int MAX_SIZE = 1000;
    char buf[MAX_SIZE] = {0};
    fmt::print(fmt::emphasis::bold | fg(fmt::color::sea_green),
               "Reading bluetooth from client.\n");

    // Buffer for series
    Action *series = nullptr;
    // Parse action
    Action a;

    // Run blocking loop to listen for bluetooth connections
    while (true) {
        // read data from the client
        int bufflen = read(client, buf, MAX_SIZE);

        // Exit function call, attempt to reconnect
        if (bufflen <= 0) {
            return;
        }

        // Override
        a = Action();

        // Parse char to string
        std::string message(buf, bufflen);

        try {
            // parse string to json
            json data = json::parse(message);
            // deserialise json to Action
            Action::from_json(data, a);
        } catch (const json::parse_error &e) {
            printRed("JSON error:");
            // Issues with fmt::format
            std::cout << e.what() << std::endl;
            //  reset series if an exception is caught
            delete series;
            series = nullptr;
            continue;
        } catch (...) {
            //  reset series if an exception is caught
            delete series;
            series = nullptr;
            printRed("Bluetooth Read: Unexpected error occurred");
            continue;
        }

        if (a.type.empty()) {
            printRed("Empty type received");
            continue;
        }

        // Check if action is a series of commands
        if (a.type.compare(Action::TYPE_SERIES) == 0) {
            series = new Action();
            *series = a;
            std::cout << series->length << std::endl;
            continue;
        }

        // series ongoing
        if (series != nullptr) {
            series->data.push_back(a);
            std::cout << series->data.size() << std::endl;
            // TODO to handle length mismatch
            if (series->data.size() != series->length) continue;

            // Data
            a = *series;
            delete series;
            series = nullptr;
            print("");
            std::cout << "Series size:" << a.data.size() << std::endl;
        }

        // map to different channels
        if (a.type.compare(Action::TYPE_MOVE) == 0)
            onAction(a);
        else if (a.type.compare(Action::TYPE_CAPTURE) == 0)
            onAction(a);
        else if (a.type.compare(Action::TYPE_SERIES) == 0)
            onSeriesActions(a);
        else if (a.type.compare(Action::TYPE_RESET) == 0) {
            resetCommands();
            resetCache();
        }
        // Publish to generic main read for debug
        // this->publish(Blueteeth::BT_MAIN_READ, buf, bufflen);
        // Clear buffer
        memset(buf, 0, sizeof(buf));
        continue;
    }
}

Blueteeth::~Blueteeth() {
    close(client);
    close(bluetoothSocket);
}
