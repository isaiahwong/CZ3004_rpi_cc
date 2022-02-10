#include "protocol.h"

#include <fmt/color.h>
#include <fmt/core.h>

void Protocol::exec(Protocol* proto) { proto->run(); }

/**
 * @brief execute sub thread with their handlers
 *
 * @param proto
 * @param sub
 * @param callback
 */
void Protocol::subexec(Protocol* proto, PubSub* sub, SubCallback callback) {
    char buf[1024];
    auto idx = sub->sub();
    while (true) {
        auto res = sub->read(idx, buf, sizeof(buf));
        if (res == PubSub::ReadOK) {
            PubSub::MsgHeader* header = (PubSub::MsgHeader*)buf;
            std::string msg = *(std::string*)(header + 1);
            callback(proto, msg);
        }
    }
}

void Protocol::subexecA(Protocol* proto, PubSub* sub,
                        SubActionCallback callback) {
    char buf[1024];
    auto idx = sub->sub();
    while (true) {
        auto res = sub->read(idx, buf, sizeof(buf));
        if (res == PubSub::ReadOK) {
            PubSub::MsgHeader* header = (PubSub::MsgHeader*)buf;
            Action* action = (Action*)(header + 1);
            callback(proto, action);
        }
    }
}

void Protocol::subexecR(Protocol* proto, PubSub* sub,
                        SubResponseCallback callback) {
    char buf[1024];
    auto idx = sub->sub();
    while (true) {
        auto res = sub->read(idx, buf, sizeof(buf));
        if (res == PubSub::ReadOK) {
            PubSub::MsgHeader* header = (PubSub::MsgHeader*)buf;
            Response* res = (Response*)(header + 1);
            callback(proto, res);
        }
    }
}

Protocol::~Protocol() {
    for (auto it = subscriptions.begin(); it != subscriptions.end(); it++) {
        delete it->second;
        subscriptions.erase(it);
    }
}

Protocol::Protocol() {
    // Reserve 10 pub sub mem slots
    pubThreads.reserve(3);
    subThreads.reserve(3);
}

void Protocol::start() {
    std::thread* t = new std::thread(Protocol::exec, this);
    mainThread.reset(t);
}

void Protocol::join() {
    for (auto& t : subThreads) {
        if (t != nullptr) {
            t->join();
            t.reset();
        }
        t = nullptr;
    }

    for (auto& t : pubThreads) {
        if (t != nullptr) {
            t->join();
            t.reset();
        }
        t = nullptr;
    }

    // Joins main thread
    if (mainThread != nullptr) {
        mainThread->join();
        mainThread.reset();
    }
    mainThread = nullptr;
}

void Protocol::registerSub(Protocol* proto, std::string channel,
                           SubResponseCallback callback) {
    if (proto == nullptr) return;
    if (channel.empty()) return;

    proto->_registerSub(proto, channel, getPub(channel), callback);
}

void Protocol::registerSub(Protocol* proto, std::string channel,
                           SubActionCallback callback) {
    if (proto == nullptr) return;
    if (channel.empty()) return;

    proto->_registerSub(proto, channel, getPub(channel), callback);
}

/**
 * @brief Assigns publisher to current class
 *
 * @param proto
 */
void Protocol::registerSub(Protocol* proto, std::string channel,
                           SubCallback callback) {
    if (proto == nullptr) return;
    if (channel.empty()) return;

    proto->_registerSub(proto, channel, getPub(channel), callback);
}

/**
 * @brief
 *
 * @param channel
 * @param sub
 * @param callback
 */
void Protocol::_registerSub(Protocol* proto, std::string channel, PubSub* sub,
                            SubResponseCallback callback) {
    if (channel.empty()) return;
    if (sub == nullptr) return;
    // start subscription with subexec callback
    std::thread* t = new std::thread(Protocol::subexecR, proto, sub, callback);
    // add to subThread
    pushThread(t);
}

/**
 * @brief
 *
 * @param channel
 * @param sub
 * @param callback
 */
void Protocol::_registerSub(Protocol* proto, std::string channel, PubSub* sub,
                            SubActionCallback callback) {
    if (channel.empty()) return;
    if (sub == nullptr) return;
    // start subscription with subexec callback
    std::thread* t = new std::thread(Protocol::subexecA, proto, sub, callback);
    // add to subThread
    pushThread(t);
}

/**
 * @brief
 *
 * @param proto
 * @param channel
 * @param sub
 * @param callback
 */
void Protocol::_registerSub(Protocol* proto, std::string channel, PubSub* sub,
                            SubCallback callback) {
    if (channel.empty()) return;
    if (sub == nullptr) return;
    // start subscription with subexec callback
    std::thread* t = new std::thread(Protocol::subexec, proto, sub, callback);
    // add to subThread
    pushThread(t);
}

/**
 * @brief appends thread to pubsub thread
 *
 * @param t
 */
void Protocol::pushThread(std::thread* t) {
    std::unique_ptr<std::thread> subThread;
    subThread.reset(t);
    subThreads.push_back(std::move(subThread));
}

/**
 * @brief Utility function that returns a thread
 *
 * @param channel
 * @return PubSub*
 */
PubSub* Protocol::getPub(std::string channel) {
    PubSub* pub;
    // Check if channel exists
    auto i = subscriptions.find(channel);
    // Create channel if not found
    if (i == subscriptions.end()) {
        pub = new PubSub();
        subscriptions.emplace(channel, pub);
    } else {
        // found publisher
        pub = i->second;
    }

    return pub;
}

/**
 * @brief
 *
 * @param channel
 * @param action
 */
void Protocol::publish(std::string channel, Action* action) {
    // Check if channel exists
    PubSub* pub = getPub(channel);
    PubSub::MsgHeader* header = pub->alloc(sizeof(Action));
    *(Action*)(header + 1) = *action;
    pub->pub();
}

/**
 * @brief
 *
 * @param channel
 * @param action
 */
void Protocol::publish(std::string channel, Response* res) {
    // Check if channel exists
    PubSub* pub = getPub(channel);
    PubSub::MsgHeader* header = pub->alloc(sizeof(Action));
    *(Response*)(header + 1) = *res;
    pub->pub();
}

void Protocol::publish(std::string channel, char* buffer, int bufflen) {
    std::string message(buffer, bufflen);
    publish(channel, message);
}

void Protocol::publish(std::string channel, std::string message) {
    // Check if channel exists
    PubSub* pub = getPub(channel);

    PubSub::MsgHeader* header = pub->alloc(sizeof(std::string));
    *(std::string*)(header + 1) = message;
    pub->pub();
}

std::string Protocol::genFnName(std::string _class, std::string fn) {
    return fmt::format("[{}]: {}", _class, fn);
}

std::string Protocol::genError(std::string prefix, std::string msg) {
    return fmt::format(fmt::emphasis::bold | fg(fmt::color::hot_pink), "{} {}",
                       prefix, msg);
}

std::string Protocol::genError(std::string prefix, std::string msg, int code) {
    return fmt::format("{} {}. ERRNO={}", prefix, msg, code);
}

void Protocol::printRed(std::string msg) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink), msg + "\n");
}

void Protocol::print(std::string msg) {
    fmt::print(fmt::emphasis::bold | fg(fmt::color::turquoise), msg + "\n");
}

void Protocol::nooploop() {
    while (true) {
    }
}
