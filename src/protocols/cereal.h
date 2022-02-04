#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <string>
#include <vector>

#include "iostream"
#include "protocol.h"

class Cereal : public Protocol {
   private:
    std::string port;

    int serial = -1;

    int baudrate;

    /**
     * @brief
     *
     */
    std::vector<std::string> hosts;

    int hostPointer = 0;

    void runWrite();
    void init();
    int connect();

   public:
    // Channels
    inline static const std::string SERIAL_MAIN_READ = "SERIAL_MAIN_READ";

    Cereal(std::string _port, int _baudrate);

    ~Cereal();

    /**
     * @brief Forwarder static function to access cereal member functions
     *
     * @param c
     * @param msg
     */
    static void onMoveAction(void* c, std::string msg);

    void onMoveAction(std::string msg);

    void run();

    void readClient();

    void writeClient(std::string msg);

    /**
     * @brief Movement fns
     * TODO refactor
     */
    void forward(std::string);

    void back(std::string);

    void left(std::string);

    void right(std::string);
};

#endif