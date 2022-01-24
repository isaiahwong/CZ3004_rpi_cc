#include "cereal.hh"

#include "iostream"

void Cereal::runRead() {
    //    int fd;
    //     if (fd = serialOpen("./virtual-tty", 9600) < 0) {
    //         std::cout << "Unable to open serial port" << std::endl;
    //         return 1;
    //     }

    //     if (wiringPiSetup() == -1) {
    //         std::cout << "Unable to start wiring pi" << std::endl;
    //         return 1;
    //     }

    //     while (serialDataAvail(fd)) {
    //         std::cout << serialGetchar(fd) << std::endl;
    //     }
    //     std::cout << "not avail" << std::endl;
}

void Cereal::runWrite() {
    // for (int i = 0; i < 10; i++) {
    //     std::this_thread::sleep_for(std::chrono::seconds(2));
    //     std::cout << "cereal" << std::endl;
    // }
}