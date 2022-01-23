/**
 * @file blueteeth.cc
 * @author isaiahwong
 * @brief
 * @version 0.1
 * @date 2022-01-23
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "blueteeth.hh"

#include <bluetooth/bluetooth.h>
#include <bluetooth/l2cap.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sco.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

#include <chrono>
#include <string>
#include <thread>

#include "iostream"

Blueteeth::Blueteeth() {
    this->name = "blueteeth";
    this->channel = 1;
    this->init();
}

Blueteeth::Blueteeth(int channel, std::string name) {
    this->name = name;
    this->channel = channel;
    this->init();
}

void Blueteeth::init() {
    std::string fn = this->genFnName(this->name, "init");
    int bluetoothSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    // RFCOMM socket address
    struct sockaddr_rc localAddr = {0};
    struct sockaddr_rc clientAddr = {0};

    // Socket length options
    socklen_t opt = sizeof(clientAddr);

    localAddr.rc_family = AF_BLUETOOTH;
    // Allows any bluetooth address to be used
    localAddr.rc_bdaddr = {0, 0, 0, 0, 0, 0};
    // Sets RFComm channel
    localAddr.rc_channel = (uint8_t)channel;

    // Creates RFComm BT Socket
    bluetoothSocket = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

    if (bluetoothSocket == -1)
        throw this->genError(fn, "Error encountered creating BT socket");

    // Binds socket to first available local bluetooth adapter
    int bindStatus =
        bind(bluetoothSocket, (struct sockaddr *)&localAddr, sizeof(localAddr));

    if (bindStatus == -1)
        throw this->genError(fn, "Error encountered creating BT socket");
}

void Blueteeth::run() {
    std::this_thread::sleep_for(std::chrono::seconds(10));
    std::cout << "Running" << std::endl;
}