#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <string>

#include "iostream"
#include "protocol.h"

class Cereal : public Protocol {
   private:
    std::string port;

    int serial = -1;

    int baudrate;

    void runWrite();
    void init();
    int connect();

   public:
    Cereal(std::string _port) : port(_port), baudrate(115200) {}

    Cereal(std::string _port, int _baudrate) : port(_port), baudrate(_baudrate) {}

    ~Cereal();

    /**
     * @brief Forwarder static function to access cereal member functions
     *
     * @param c
     * @param msg
     */
    static void onCommand(void* c, std::string msg);

    void onCommand(std::string msg);

    void run();

    void readClient();

    void writeClient(std::string msg);
};

#endif