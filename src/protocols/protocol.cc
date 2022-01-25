#include "protocol.hh"

#include <fmt/color.h>
#include <fmt/core.h>

void Protocol::exec(Protocol* proto) { proto->run(); }

void Protocol::subexec(Protocol* proto, PubSub* sub, SubCallback callback) {
    char buf[100];
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

Protocol::~Protocol() {
    for (auto it = subscriptions.begin(); it != subscriptions.end(); it++) {
        delete it->second;
        subscriptions.erase(it);
    }
}

Protocol::Protocol() {
    // Reserve 10 pub sub mem slots
    pubThreads.reserve(10);
    subThreads.reserve(10);
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

/**
 * @brief Assigns publisher to current class
 *
 * @param proto
 */
void Protocol::subscribe(Protocol* proto, std::string channel,
                         SubCallback callback) {
    if (proto == nullptr) return;
    if (channel.empty()) return;

    PubSub* pub;
    // Check if channel exists
    auto i = subscriptions.find(channel);
    if (i == subscriptions.end()) {  // Create channel if not found
        pub = new PubSub();
        subscriptions.emplace(channel, pub);
    } else {
        // found publisher
        pub = i->second;
    }

    proto->_subscribe(proto, channel, pub, callback);
}

/**
 * @brief
 *
 * @param channel
 * @param sub
 * @param callback
 */
void Protocol::_subscribe(Protocol* proto, std::string channel, PubSub* sub,
                          SubCallback callback) {
    if (channel.empty()) return;
    if (sub == nullptr) return;

    // start subscription with subexec callback
    std::thread* t = new std::thread(Protocol::subexec, proto, sub, callback);
    // Create a subThread
    std::unique_ptr<std::thread> subThread;
    subThread.reset(t);
    subThreads.push_back(std::move(subThread));
}

void Protocol::publish(std::string channel, char* buffer, int bufflen) {
    std::string message(buffer, bufflen);
    publish(channel, message);
}

void Protocol::publish(std::string channel, std::string message) {
    // Check if channel exists
    PubSub* pub;

    auto i = subscriptions.find(channel);
    if (i == subscriptions.end()) {  // Create channel if not found
        fmt::print(fmt::emphasis::bold | fg(fmt::color::hot_pink),
                   "Channel does not exist\n");
        return;
    } else {
        // found publisher
        pub = i->second;
    }

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
