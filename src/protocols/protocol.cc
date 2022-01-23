#include "protocol.hh"

#include <fmt/core.h>

void Protocol::exec(Protocol* pThread) { pThread->run(); }

// Run thread protocol
void Protocol::start() {
    std::thread* t = new std::thread(Protocol::exec, this);
    pThread.reset(t);
}

void Protocol::join() {
    if (pThread == nullptr) return;
    pThread->join();
    pThread = nullptr;
}

std::string Protocol::genFnName(std::string _class, std::string fn) {
    return fmt::format("[{}]: {}", _class, fn);
}

std::string Protocol::genError(std::string prefix, std::string msg) {
    return fmt::format("{} {}", prefix, msg);
}