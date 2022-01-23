#include "protocol.hh"

#include <format>

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

std::string genFnName(std::string _class, std::string fn) {
    return std::format("[{}]: {}", _class, fn);
}

std::string genError(std::string prefix, std::string msg) {
    return std::format("{} {}", prefix, msg);
}