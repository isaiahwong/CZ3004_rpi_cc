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
    while (true) {
        connect();
        this->readClient();
    }
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

void Blueteeth::onResponse(void *b, Response *res) {
    static_cast<Blueteeth *>(b)->onResponse(res);
}

void Blueteeth::onResponse(Response *res) {
    if (res == nullptr) return;
    json j;
    res->to_json(j);
    std::string payload = j.dump();

    int size = payload.size();

    // Write back to client
    write(client, payload.c_str(), size);
}

/**
 * @brief Publishes incoming bluetooth connections
 *
 */
void Blueteeth::readClient() {
    // Wait for bluetooth client to accept
    char buf[1024] = {0};
    fmt::print(fmt::emphasis::bold | fg(fmt::color::sea_green),
               "Reading bluetooth from client.\n");
    // Run blocking loop to listen for bluetooth connections
    while (true) {
        // read data from the client
        int bufflen = read(client, buf, sizeof(buf));

        // Exit function call, attempt to reconnect
        if (bufflen <= 0) {
            return;
        }

        // Parse char to string
        std::string message(buf, bufflen);

        // Parse action
        Action a;

        try {
            // parse string to json
            json data = json::parse(message);
            // deserialise json to Action
            Action::from_json(data, a);
        } catch (const json::parse_error &e) {
            // Issues with fmt::format
            std::cout << e.what() << std::endl;
            continue;
        } catch (...) {
            printRed("Bluetooth Read: Unexpected error occurred");
            continue;
        }

        if (a.type.empty()) {
            printRed("Empty type received");
            continue;
        }

        if (a.action.empty()) {
            printRed("Empty action received");
            continue;
        }

        // If there's size
        if (a.data.size()) {
        } else {
        }

        // map to different channels
        if (a.type.compare(Action::TYPE_MOVE) == 0)
            this->publish(Blueteeth::BT_MOVEMENT, &a);
        else if (a.type.compare(Action::TYPE_CAPTURE) == 0)
            this->publish(Blueteeth::BT_CAMERA_CAPTURE, &a);

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