

#include <cerrno>
#include <cstring>
#include <iostream>
#include <string>
#include <signal.h>

#include "opencv2/opencv.hpp"
#include "protocols/camera.h"
#include "protocols/blueteeth.h"
#include "protocols/cereal.h"

void test(std::string msg) {}

int main() {
    Blueteeth bt;
    Cereal c;
    Camera cam;

    bt.registerSub(&c, Blueteeth::BT_SERIAL_SEND, Cereal::onCommand);
    bt.registerSub(&cam, Blueteeth::BT_CAMERA_VIDEO, Camera::onVideoOpen);


    bt.start();
    c.start();
    cam.start();
    
    c.join();
    bt.join();
    cam.join();

    return 0;
}
