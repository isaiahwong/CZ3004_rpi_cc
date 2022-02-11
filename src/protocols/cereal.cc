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
void Cereal::onAction(void* c, Action* action) {
    static_cast<Cereal*>(c)->onAction(action);
}

void Cereal::onAction(Action* action) {
    Action a = *action;
    try {
        // Switch cases
        if (a.action.compare(Movement::FORWARD) == 0)
            movement.forward(a, this, writeClient);
        else if (a.action.compare(Movement::BACK) == 0)
            movement.back(a, this, writeClient);
        else if (a.action.compare(Movement::FORWARD_LEFT) == 0)
            movement.forwardLeft(a, this, writeClient);
        else if (a.action.compare(Movement::FORWARD_RIGHT) == 0)
            movement.forwardRight(a, this, writeClient);
        else if (a.action.compare(Movement::BACK_LEFT) == 0)
            movement.backLeft(a, this, writeClient);
        else if (a.action.compare(Movement::BACK_RIGHT) == 0)
            movement.backRight(a, this, writeClient);
        else if (a.action.compare(Movement::STOP) == 0)
            movement.stop(a, this, writeClient);
        else if (a.action.compare(Movement::CENTER) == 0)
            movement.center(a, this, writeClient);
    } catch (...) {
    }
}

/**
 * @brief static function to forward
 *
 * @param c
 * @param msg
 */
void Cereal::writeClient(void* c, std::string msg) {
    static_cast<Cereal*>(c)->writeClient(msg);
}

/**
 * @brief Write to STM
 *
 * @param msg
 */
void Cereal::writeClient(std::string msg) {
    const int LEN = 5;
    char buf[LEN];

    try {
        // Truncates till 4
        strncpy(buf, msg.c_str(), LEN);
        buf[LEN - 1] = '0';
        // buf[LEN - 1] = '\0';
        std::cout << buf << std::endl;
        serialPuts(serial, buf);
    } catch (std::exception& e) {
        std::cout << "Exception caught : " << e.what() << std::endl;
    }
}

void Cereal::run() {
    while (true) {
        // if connect fails
        if (connect()) {
            fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                       "Retrying in 5 seconds\n");
            std::this_thread::sleep_for(std::chrono::seconds(2));
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
            try {
                // Overflow from buffer
                if (sLen >= 1024) sLen = 1023;
                buf[sLen] = '\0';
                // TODO: map SERIAL RESPONSE AND SEND TO ANDROID
                std::cout << buf << sLen << std::endl;
                // Publish Serial in
                // this->publish(Cereal::SERIAL_MAIN_READ, buf, sLen);
                // Clear buffer
                memset(buf, 0, sizeof(buf));
                // Reset length
                sLen = 0;
            } catch (std::exception& e) {
                std::cout << "Exception caught : " << e.what() << std::endl;
            }

        } else {
            // Build String and it into serial_buf
            buf[sLen++] = c;
        }
    }
}