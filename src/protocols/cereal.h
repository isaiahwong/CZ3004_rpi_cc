#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <string>

#include "iostream"
#include "protocol.h"

class Cereal : public Protocol {
   private:
    int serial = -1;

    void runWrite();
    void init();
    int connect();

   public:
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