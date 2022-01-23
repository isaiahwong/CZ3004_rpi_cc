#include "cereal.hh"

#include "iostream"

void Cereal::run() {
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "cereal" << std::endl;
    }
}