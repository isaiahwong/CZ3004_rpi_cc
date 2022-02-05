

#include <signal.h>

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>

#include "common/cmdline.h"
#include "opencv2/opencv.hpp"
#include "protocols/blueteeth.h"
#include "protocols/camera.h"
#include "protocols/cereal.h"

void test(std::string msg) {}

int main(int argc, char *argv[]) {
    // create a parser
    cmdline::parser p;

    // add specified type of variable.
    // 1st argument is long name
    // 2nd argument is short name (no short name if '\0' specified)
    // 3rd argument is description
    // 4th argument is mandatory (optional. default is false)
    // 5th argument is default value  (optional. it used when mandatory is
    // false)
    p.add<std::string>("serial", 's', "serial port", true, "/dev/tty1");
    p.add<std::string>("vision", 'v', "vision address", true,
                       "localhost:50051");

    // Run parser.
    // It returns only if command line arguments are valid.
    // If arguments are invalid, a parser output error msgs then exit program.
    p.parse_check(argc, argv);

    Blueteeth bt;
    Cereal c(p.get<std::string>("serial"), 115200);
    Camera cam(p.get<std::string>("vision"));

    bt.registerSub(&c, Blueteeth::BT_MAIN_READ, Cereal::onAction);
    c.registerSub(&cam, Cereal::SERIAL_MAIN_READ, Camera::onCapture);

    bt.start();
    c.start();
    cam.start();

    c.join();
    bt.join();
    cam.join();

    return 0;
}
