#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <blockingconcurrentqueue.h>

#include <string>
#include <vector>

#include "action.h"
#include "iostream"
#include "movement.h"
#include "protocol.h"

using Queue = moodycamel::BlockingConcurrentQueue<Action&>;

class Cereal : public Protocol {
   private:
    std::string port;

    int serial = -1;

    int baudrate;

    /**
     * @brief Movement helper class for STM movements
     *
     */
    Movement movement;

    /**
     * @brief Blocking queue of commands
     *
     */
    Queue commands;

    std::atomic<bool> write = false;

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
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onConnectionRead(void* c);

    void onConnectionRead();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onAction(void* c, Action* action);

    void onAction(Action* action);

    void run();

    void readClient();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void writeClient(void* c, std::string msg);

    void writeClient(std::string msg);
};

#endif