#include "cereal.h"

#include <bits/stdc++.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

Cereal::Cereal(std::string _port, int _baudrate = 115200) {
    port = _port;
    baudrate = _baudrate;

    // split strings
    std::stringstream ss(_port);
    std::string word;
    while (ss >> word) hosts.push_back(word);
}

Cereal::~Cereal() { close(serial); }

/**
 * @brief static function to forward
 *
 * @param c
 * @param msg
 */
void Cereal::onMoveAction(void* c, std::string msg) {
    static_cast<Cereal*>(c)->onMoveAction(msg);
}

void Cereal::onMoveAction(std::string msg) {
    const int LEN = 4;
    char* buf;
    char command;
    strncpy(buf, msg.c_str(), LEN);
    command = buf[0];

    switch (command) {
        case 'F':
            forward(msg);
            break;
        case 'B':
            back(msg);
            break;
        case 'L':
            left(msg);
            break;
        case 'R':
            right(msg);
            break;
    }
}

void Cereal::writeClient(std::string msg) {
    const int LEN = 4;
    char buf[LEN];
    // Truncates till 4
    strncpy(buf, msg.c_str(), LEN);
    buf[LEN - 1] = '\0';
    serialPuts(serial, buf);
}

void Cereal::forward(std::string command) { writeClient(command); }

void Cereal::back(std::string command) { writeClient(command); }

void Cereal::left(std::string command) {
    writeClient("A000");
    writeClient("W028");
}

void Cereal::right(std::string command) {
    writeClient("D000");
    writeClient("W057");
}

void Cereal::run() {
    while (true) {
        // if connect fails
        if (connect()) {
            fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                       "Retrying in 5 seconds\n");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            continue;
        }
        this->readClient();
    }
}

void Cereal::init() {}

int Cereal::connect() {
    // Get the current host pointer and atttempt to connect
    if ((serial = serialOpen(hosts[hostPointer].c_str(), baudrate)) < 0) {
        printRed(fmt::format("Unable to open serial device {}: {} \n",
                             hosts[hostPointer], strerror(errno)));
        // Rotate to the next host
        hostPointer = (hostPointer + 1) % hosts.size();
        return 1;
    }
    return 0;
}

void Cereal::readClient() {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::lime_green),
               "Reading serial from client.\n");

    // Clear
    char c;
    int sLen = 0;
    char buf[1024];

    serialFlush(serial);

    while (true) {
        // Checks if serial
        if (serialDataAvail(serial) < 0) {
            fmt::print("Serial disconnected\n");
            return;
        }

        // Get char from serial
        c = serialGetchar(serial);

        // No message
        if (c == -1) {
            continue;
        }

        // Possible character termination
        if (c == '\n' || c == '\r' || c == '\0') {
            // Overflow from buffer
            if (sLen >= 1024) sLen = 1023;
            buf[sLen] = '\0';
            // Publish Serial in
            this->publish(Cereal::SERIAL_MAIN_READ, buf, sLen);
            // Clear buffer
            memset(buf, 0, sizeof(buf));
            // Reset length
            sLen = 0;
        } else {
            // Build String and it into serial_buf
            buf[sLen++] = c;
        }
    }
}