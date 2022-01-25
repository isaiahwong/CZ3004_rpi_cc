

#include <wiringPi.h>
#include <wiringSerial.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include "opencv2/opencv.hpp"
#include "protocols/blueteeth.hh"
#include "protocols/cereal.hh"

int main() {
    Blueteeth bt;
    //    Cereal c;

    bt.start();
    // c.start();
    // c.join();
    bt.join();
    return 0;
}
