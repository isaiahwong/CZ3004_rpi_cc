

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
#include "protocols/commands.h"
#include "protocols/htttp.h"

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
    p.add<std::string>("serial", 'a', "serial port", true, "/dev/tty1");
    p.add<int>("serialretry", 'b', "serial retry", false, 1);
    p.add<int>("cameraopen", 'c', "camera should open", false, 1);
    p.add<std::string>("vision", 'd', "vision address", true,
                       "localhost:50051");
    p.add<int>("instructiondelay", 'e', "instruction delay", false, 350);
    p.add<int>("failcapturedelay", 'f', "Camera capture delay", false, 5000);
    p.add<int>("captureretries", 'g', "Camera fail capture retries", false, 5);

    // Run parser.
    // It returns only if command line arguments are valid.
    // If arguments are invalid, a parser output error msgs then exit program.
    p.parse_check(argc, argv);

    Blueteeth bt;
    Htttp h;
    Commands cmd(p.get<int>("instructiondelay"), p.get<int>("failcapturedelay"),
                 p.get<int>("captureretries"));
    Cereal c(p.get<std::string>("serial"), 115200);
    Camera cam(p.get<std::string>("vision"), p.get<int>("cameraopen"));

    // Register cereal to listen for movement requests
    cmd.registerSub(&c, Commands::CMD_MOVEMENT, Cereal::onAction);
    // Register camera to listen for camera requests
    cmd.registerSub(&cam, Commands::CMD_CAMERA_CAPTURE, Camera::onCapture);

    cmd.registerSub(&bt, Commands::CMD_RESPONSE, Blueteeth::onWrite);

    // Register read from HTTP
    h.registerSub(&cmd, Htttp::HTTTP_SERIES_ACTIONS, Commands::onSeriesActions);
    h.registerSub(&cmd, Htttp::HTTTP_ACTION, Commands::onAction);
    h.registerSub(&cmd, Htttp::HTTTP_CMD_RESET, Commands::onReset);
    h.registerSub(&cmd, Htttp::HTTTP_SERIES_INTERLEAVE,
                  Commands::onSeriesInterleave);

    // Register read from bluetooth
    bt.registerSub(&cmd, Blueteeth::BT_SERIES_ACTIONS,
                   Commands::onSeriesActions);
    bt.registerSub(&cmd, Blueteeth::BT_ACTION, Commands::onAction);
    bt.registerSub(&cmd, Blueteeth::BT_CMD_RESET, Commands::onReset);

    // Register camera on response
    cam.registerSub(&cmd, Camera::CAM_CAPTURE_RESULT, Commands::onResponse);
    // Register cereal to listen for movement calls
    c.registerSub(&cmd, Cereal::SERIAL_MAIN_WRITE_SUCCESS,
                  Commands::onResponse);
    // c.registerSub(&c, Cereal::SERIAL_MAIN_READ, Cereal::onAction);

    // Start the respective protocols
    bt.start();
    c.start();
    cam.start();
    cmd.start();
    h.start();

    // Blocking wait
    c.join();
    bt.join();
    cam.join();
    cmd.join();
    h.join();

    return 0;
}
