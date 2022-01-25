#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <string>

#include "iostream"
#include "protocol.hh"

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
    static void listenBT(void* c, std::string msg);
    void listenBT(std::string msg);

    void readClient();
    void run();
};

#endif