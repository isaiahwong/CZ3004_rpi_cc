#pragma once
#ifndef BLUETEETH_H_
#define BLUETEETH_H_

#include "protocol.h"

class Blueteeth final : public Protocol {
   private:
    std::string name = "blueteeth";
    int channel = 1;

    // RFCOMM socket address
    struct sockaddr_rc *clientAddr;
    struct sockaddr_rc *localAddr;

    // bluetooth socket
    int bluetoothSocket = -1;

    // bluetooth client
    int client = -1;

    void init();
    void connect();
    void disconnect();
    void readClient();

   public:
    // Channels
    inline static const std::string BT_MAIN_READ = "BT_MAIN_READ";

    inline static const std::string BT_CAMERA_CAPTURE = "BT_CAMERA_CAPTURE";

    ~Blueteeth();
    Blueteeth();
    void run();
    Blueteeth(int channel, std::string name);
};

#endif
