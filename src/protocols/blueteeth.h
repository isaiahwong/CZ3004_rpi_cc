#pragma once
#ifndef BLUETEETH_H_
#define BLUETEETH_H_

#include <blockingconcurrentqueue.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

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

    // Default addresses
    bdaddr_t ANY = {0, 0, 0, 0, 0, 0};
    bdaddr_t LOCAL = {0, 0, 0, 0xff, 0xff, 0xff};

    // bluetooth socket
    int bluetoothSocket = -1;

    // bluetooth client
    int client = -1;

    int instructionDelay = 500;

    /**
     * @brief Blocking queue of commands
     *
     */
    BlockingQueueAction commands;

    /**
     * @brief Store commands temporarily
     *
     */
    BlockingQueueAction cached;

    BlockingQueueRes statuses;

    void init();
    void connect();
    void disconnect();
    void readClient();
    void emptyCommands();
    void cameraStrategy();
    void resetCommands();
    void resetCache();

   public:
    // Channels
    inline static const std::string BT_MAIN_READ = "BT_MAIN_READ";
    inline static const std::string BT_MAIN_WRITE = "BT_MAIN_WRITE";
    inline static const std::string BT_ACTION = "BT_ACTION";
    inline static const std::string BT_SERIES_ACTIONS = "BT_SERIES_ACTIONS";
    inline static const std::string BT_CMD_RESET = "BT_CMD_RESET";

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

    static void onWrite(void* c, Response* response);

    void onWrite(Response* response);

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onExecuteActions(void* b);

    void onExecuteActions();

    void onSeriesActions(Action& action);

    void onAction(Action& action);

    void onReset();

    void setDelay(int instructiondelay) {
        this->instructionDelay = instructiondelay;
    }

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onConnectionRead(void* b);

    void onConnectionRead();

    ~Blueteeth();
    Blueteeth();
    Blueteeth(int channel, std::string name);
    Blueteeth(int channel, std::string name, int instructionDelay);
    void run();
};

#endif
