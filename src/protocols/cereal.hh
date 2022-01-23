#ifndef CEREAL_H_
#define CEREAL_H_

#include "iostream"
#include "protocol.hh"

class Cereal : public Protocol {
   private:
    void run();
};

#endif