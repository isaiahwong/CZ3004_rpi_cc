#ifndef BLUETEETH_H_
#define BLUETEETH_H_

#include "protocol.hh"

class Blueteeth : public Protocol {
   private:
    std::string name = "blueteeth";
    int channel = 1;

    void run();
    void init();

   public:
    Blueteeth();
    Blueteeth(int channel, std::string name);
};

#endif