#ifndef BLUETEETH
#define BLUETEETH

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