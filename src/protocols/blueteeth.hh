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

    void runRead();
    void runWrite();
    void init();
    void connect();

   public:
    ~Blueteeth();
    Blueteeth();
    Blueteeth(int channel, std::string name);
};

#endif