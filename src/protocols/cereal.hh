#ifndef CEREAL
#define CEREAL

#include "iostream"
#include "protocol.hh"

class Cereal : public Protocol {
   private:
    void run();
};

#endif