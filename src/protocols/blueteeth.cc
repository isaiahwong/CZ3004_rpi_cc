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

#include "blueteeth.hh"

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
#include <string>
#include <thread>

#include "iostream"

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

void Blueteeth::disconnect() {
    if (!close(client) && !close(bluetoothSocket))
        fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                   "BT closed\n");
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
        int bytes_read = read(client, buf, sizeof(buf));

        // Exit function call, attempt to reconnect
        if (bytes_read <= 0) {
            return;
        }

        this->publish(Blueteeth::BT_SERIAL_SEND, buf, bytes_read);
        // Clear buffer
        memset(buf, 0, sizeof(buf));
        continue;
    }
}

Blueteeth::~Blueteeth() {
    close(client);
    close(bluetoothSocket);
}