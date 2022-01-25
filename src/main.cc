

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"
#include "protocols/blueteeth.hh"
#include "protocols/cereal.hh"

void test(std::string msg) {}

int main() {
    Blueteeth bt;
    Cereal c;
    bt.subscribe(&c, "CLIENT", Cereal::listenBT);

    bt.start();
    c.start();
    c.join();
    bt.join();
    return 0;
}
