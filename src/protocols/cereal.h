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

using BlockingQueueAction = moodycamel::BlockingConcurrentQueue<Action>;
using BlockingQueueStatus = moodycamel::BlockingConcurrentQueue<int>;

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
    // BlockingQueueAction commands;

    // BlockingQueueStatus statuses;

    std::atomic<bool> write;

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
    inline static const std::string SERIAL_MAIN_WRITE_SUCCESS =
        "SERIAL_MAIN_WRITE_SUCCESS";

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
    static void onExecuteActions(void* c);

    void onExecuteActions();

    /**
     * @brief Function to determine if write to stm is allowed
     *
     * @return true
     * @return false
     */
    bool canWrite();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onAction(void* c, Action* action);

    void onAction(Action* action);

    // void onActions(Action* action);

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