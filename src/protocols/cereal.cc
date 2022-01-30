#include "cereal.h"

#include <fmt/color.h>
#include <fmt/core.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

Cereal::~Cereal() { close(serial); }

/**
 * @brief static function to forward
 *
 * @param c
 * @param msg
 */
void Cereal::onCommand(void* c, std::string msg) {
    static_cast<Cereal*>(c)->onCommand(msg);
}

void Cereal::onCommand(std::string msg) {
    // TODO, send messages based on enums
    writeClient(msg);
}

void Cereal::writeClient(std::string msg) {
    const int LEN = 4;
    char buf[LEN];
    strncpy(buf, msg.c_str(), LEN);
    buf[LEN - 1] = '\0';
    serialPuts(serial, buf);
    serialFlush(serial);
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
    if ((serial = serialOpen(port.c_str(), baudrate)) < 0) {
        fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                   "Unable to open serial device: {} \n", strerror(errno));
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