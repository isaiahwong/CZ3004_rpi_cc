#include "protocol.hh"

#include <fmt/core.h>

void Protocol::exec(Protocol* proto, Protocol::ThreadOp op) {
    switch (op) {
        case Protocol::READ:
            proto->runRead();
            break;
        case Protocol::WRITE:
            proto->runWrite();
            break;
    }
}

Protocol::Protocol() { pub = new PubSub(); }

void Protocol::setSub(PubSub* sub) { this->sub = sub; }

// Run thread protocol
void Protocol::start() {
    // Spawn threads for read and write threads
    std::thread* rt = new std::thread(Protocol::exec, this, Protocol::READ);
    std::thread* wt = new std::thread(Protocol::exec, this, Protocol::WRITE);

    // assign pointer to smart pointer
    readThread.reset(rt);
    writeThread.reset(wt);
}

void Protocol::join() {
    if (readThread != nullptr) readThread->join();
    if (writeThread != nullptr) writeThread->join();
    readThread = nullptr;
    writeThread = nullptr;
}

/**
 * @brief Assigns the publisher of the current class
 *
 * @param proto
 */
void Protocol::subscribe(Protocol* proto) { proto->setSub(this->pub); }

void Protocol::publish(char* buffer, int bufflen) {
    std::string message(buffer, bufflen);
    publish(message);
}

void Protocol::publish(std::string message) {
    PubSub::MsgHeader* header = pub->alloc(sizeof(std::string));
    *(std::string*)(header + 1) = message;
    pub->pub();
}

std::string Protocol::genFnName(std::string _class, std::string fn) {
    return fmt::format("[{}]: {}", _class, fn);
}

std::string Protocol::genError(std::string prefix, std::string msg) {
    return fmt::format("{} {}", prefix, msg);
}

std::string Protocol::genError(std::string prefix, std::string msg, int code) {
    return fmt::format("{} {}. ERRNO={}", prefix, msg, code);
}
