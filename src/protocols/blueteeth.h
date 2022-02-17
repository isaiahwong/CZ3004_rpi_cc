#pragma once
#ifndef BLUETEETH_H_
#define BLUETEETH_H_

#include <blockingconcurrentqueue.h>

#include "protocol.h"

using BlockingQueueAction = moodycamel::BlockingConcurrentQueue<Action>;
using BlockingQueueRes = moodycamel::BlockingConcurrentQueue<Response>;

class Blueteeth final : public Protocol {
   private:
    std::string name = "blueteeth";
    int channel = 1;

    // RFCOMM socket address
    struct sockaddr_rc* clientAddr;
    struct sockaddr_rc* localAddr;

    // bluetooth socket
    int bluetoothSocket = -1;

    // bluetooth client
    int client = -1;

    /**
     * @brief Blocking queue of commands
     *
     */
    BlockingQueueAction commands;

    BlockingQueueRes statuses;

    void init();
    void connect();
    void disconnect();
    void readClient();

   public:
    // Channels
    inline static const std::string BT_MAIN_READ = "BT_MAIN_READ";
    inline static const std::string BT_CAMERA_CAPTURE = "BT_CAMERA_CAPTURE";
    inline static const std::string BT_MOVEMENT = "BT_MOVEMENT";

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onResponse(void* c, Response* response);

    void onResponse(Response* response);

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onExecuteActions(void* c);

    void onExecuteActions();

    void onActions(Action& action);

    void onAction(Action& action);

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onConnectionRead(void* c);

    void onConnectionRead();

    ~Blueteeth();
    Blueteeth();
    void run();
    Blueteeth(int channel, std::string name);
};

#endif
