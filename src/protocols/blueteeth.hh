#pragma once
#ifndef BLUETEETH_H_
#define BLUETEETH_H_

#include "protocol.hh"

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
    inline static const std::string BT_SERIAL_SEND = "BT_SERIAL_SEND";
    inline static const std::string BT_CAMERA_VIDEO = "BT_CAMERA_VIDEO";

    ~Blueteeth();
    Blueteeth();
    void run();
    Blueteeth(int channel, std::string name);
};

#endif