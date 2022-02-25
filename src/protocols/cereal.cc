#include "cereal.h"

#include <bits/stdc++.h>
#include <fmt/color.h>
#include <fmt/core.h>
#include <termios.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <chrono>
#include <iostream>
#include <string>
#include <thread>

Cereal::Cereal(std::string _port, int _baudrate = 115200) {
    write = false;
    port = _port;
    baudrate = _baudrate;
    // split strings
    std::stringstream ss(_port);
    std::string word;
    // stub_ = SerialService::NewStub(grpc::CreateChannel(
    //     "localhost:50052", grpc::InsecureChannelCredentials()));
    // commands = BlockingQueueAction();
    // statuses = BlockingQueueStatus();
    while (ss >> word) hosts.push_back(word);
}

Cereal::Cereal(std::string _port, int _baudrate, std::string fr, std::string br,
               std::string fl, std::string bl) {
    write = false;
    port = _port;
    baudrate = _baudrate;
    movement = Movement(fr, br, fl, bl);

    // split strings
    std::stringstream ss(_port);
    std::string word;
    while (ss >> word) hosts.push_back(word);
}

Cereal::~Cereal() { close(serial); }

void Cereal::run() { init(); }

void Cereal::init() {
    // Create connection Read Thread
    std::thread* connThread = new std::thread(
        static_cast<void (*)(void* c)>(Cereal::onConnectionRead), this);
    // std::thread* receiveActionsThread = new std::thread(
    //     static_cast<void (*)(void* c)>(Cereal::onExecuteActions), this);

    // Push to protocol subthreads
    subThreads.push_back(UniqueThreadPtr(connThread));
    // subThreads.push_back(UniqueThreadPtr(receiveActionsThread));
}

void Cereal::onConnectionRead(void* c) {
    static_cast<Cereal*>(c)->onConnectionRead();
}

void Cereal::onConnectionRead() {
    // ClientContext context;

    // Create reader
    // reader = std::unique_ptr<ClientReader<SerialResponse> >(
    //     stub_->SendResponse(&context, Empty()));

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

void Cereal::onExecuteActions(void* c) {
    static_cast<Cereal*>(c)->onExecuteActions();
}

/**
 * @brief Waits for commands queue
 *
 */
void Cereal::onExecuteActions() {
    // Action a;
    // int status, retries = 0, MAX_RETRIES = 3;
    // while (true) {
    //     commands.wait_dequeue(a);
    //     print("Done waiting");

    //     while (true) {
    //         onAction(&a);

    //         bool didReceive =
    //             statuses.wait_dequeue_timed(status, std::chrono::seconds(5));

    //         // retry loop if failed
    //         if (!didReceive || status != 1) continue;
    //         // Break queue if successful
    //         break;
    //     }

    //     print("Success");
    //     // send action to android to notify success
    //     Response response("", status, a.coordinate);
    //     this->publish(SERIAL_MAIN_WRITE_SUCCESS, response);
    // }
}

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
    if (action == nullptr) return;
    Action a = *action;
    print(a.coordinate);
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
            movement.stop(this, writeClient);
        else if (a.action.compare(Movement::CENTER) == 0)
            movement.center(this, writeClient);
    } catch (const std::exception& exc) {
        printRed("onAction: ");
        std::cout << exc.what() << std::endl;
    }
}

// void Cereal::onSeriesActions(Action* action) {
//     if (action == nullptr || action->data.size() < 1) return;
//     Action remove;
//     // Override existing queue
//     // Empty queue
//     while (commands.try_dequeue(remove))
//         ;
//     // Enqueue
//     for (Action a : action->data) {
//         commands.enqueue(a);
//     }
// }

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
    const int LEN = 9;
    char buf[LEN];
    // ClientContext context;

    try {
        // Truncates till LEN
        strncpy(buf, msg.c_str(), LEN);
        std::cout << buf << std::endl;
        // buf[LEN - 1] = '\0';
        // SerialMessage s;
        // Empty e;
        // s.mutable_message()->assign(msg);
        // print(msg);
        // Status status = stub_->SendMessage(&context, s, &e);

        // print("done");
        serialPuts(serial, buf);
    } catch (std::exception& e) {
        std::cout << "Exception caught : " << e.what() << std::endl;
    }
}

bool Cereal::canWrite() { return true; }

int Cereal::connect() {
    // Get the current host pointer and atttempt to con nect
    if ((serial = serialOpen(hosts[hostPointer].c_str(), baudrate)) < 0) {
        printRed(fmt::format("Unable to open serial device {}: {} \n",
                             hosts[hostPointer], strerror(errno)));
        // Rotate to the next host
        hostPointer = (hostPointer + 1) % hosts.size();
        return 1;
    }

    struct termios options;
    tcgetattr(serial, &options);     // Read current options
    options.c_cflag &= ~CSIZE;       // Mask out size
    options.c_cflag &= ~CSTOPB;      // 1 stop bit
    options.c_cflag |= CS8;          // Or in 8-bits
    options.c_cflag |= PARENB;       // Enable Parity - even by default
    tcsetattr(serial, 0, &options);  // Set new options
    return 0;
}

void Cereal::readClient() {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::lime_green),
               "Reading serial from client.\n");

    // Clear
    char c;
    int sLen = 0;
    int MAX_BUFFER = 2;
    char buf[MAX_BUFFER];

    SerialResponse response;
    Response res;

    serialFlush(serial);

    // Grpc
    // while (reader->Read(&response)) {
    //     std::string strStatus = response.status();
    //     std::cout << strStatus << std::endl;
    //     char status = strStatus[0] - '0';
    //     res = Response("", status, "");
    //     this->publish(Cereal::SERIAL_MAIN_WRITE_SUCCESS, res);
    // }

    while (true) {
        // Checks if serial
        if (serialDataAvail(serial) < 0) {
            fmt::print("Serial disconnected\n");
            return;
        }

        // Get char from serial
        c = serialGetchar(serial);

        // No message
        if (c == -1 || c >= 255) {
            serialFlush(serial);
            continue;
        }

        // Possible character termination
        if (c == '\n' || c == '\r' || c == '\0') {
            try {
                // Overflow from buffer
                if (sLen >= MAX_BUFFER) sLen = MAX_BUFFER - 1;
                buf[sLen] = '\0';
                print("");
                std::cout << "STM: " << buf << std::endl;
                int status = buf[0] - '0';

                // Fill with filler messages
                Response res(status);
                // Notify status of message
                // statuses.enqueue(status);
                // Publish Serial in
                this->publish(Cereal::SERIAL_MAIN_WRITE_SUCCESS, res);
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