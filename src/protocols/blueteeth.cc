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

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
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
    (*localAddr).rc_bdaddr = {0, 0, 0, 0, 0, 0};
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

    int retries = 0, MAX_RETRIES = 5;
    while (true) {
    queue:
        commands.wait_dequeue(a);
        retries = 0;
        while (true) {
            if (a.type.compare(Action::TYPE_MOVE) == 0) {
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                this->publish(Blueteeth::BT_MOVEMENT, a);
            } else if (a.type.compare(Action::TYPE_CAPTURE) == 0) {
                this->publish(Blueteeth::BT_CAMERA_CAPTURE, a);
            } else {
                printRed("Unknown command in series");
                goto queue;
            }

            try {
                bool didReceive = statuses.wait_dequeue_timed(
                    statusResponse, std::chrono::seconds(6));

                // retry loop if failed
                if (!didReceive || statusResponse.status != 1) {
                    printRed("No Response");
                    // Retry only for Image Rec
                    if (a.type.compare(Action::TYPE_CAPTURE) != 0) break;

                    if (retries >= MAX_RETRIES) {
                        printRed("Max retries, skipping command");
                        statusResponse.status = 0;
                        goto queue;
                    }
                    retries++;
                    // std::this_thread::sleep_for(std::chrono::seconds(2));
                    continue;
                }
                // Break queue if successful
                break;
            } catch (const std::exception &exc) {
                printRed("onExecuteActions Exeception: ");
                std::cout << exc.what() << std::endl;
                break;
            }
        }

        try {
            // Send response to android to notify success
            // Echo back the coordinate given
            Response response(a.type, statusResponse.result,
                              statusResponse.status, a.coordinate);
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
            break;
        }
    }
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

/**
 * @brief Clear command queue
 *
 */
void Blueteeth::emptyCommands() {
    Action remove;
    // Override existing queue
    // Empty queue
    while (commands.try_dequeue(remove))
        ;
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
    emptyCommands();
    commands.enqueue(action);
}

void Blueteeth::onSeriesActions(Action &action) {
    if (action.data.size() < 1) return;
    emptyCommands();
    // Enqueue
    for (Action a : action.data) {
        commands.enqueue(a);
    }
}

void Blueteeth::onBullseye() {
    emptyCommands();
    // To fill tomorrow
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

        // series on going
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
        else if (a.type.compare(Action::TYPE_BULLSEYE) == 0)
            onBullseye();
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
