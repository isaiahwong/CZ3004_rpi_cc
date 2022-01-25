#pragma once

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <map>
#include <string>
#include <thread>
#include <vector>

#include "pubsub.hh"

using PubSub = PubSubQueue<1024>;
using UniquePtr = std::unique_ptr<std::thread>;
using VectorThread = std::vector<UniquePtr>;

typedef void (*SubCallback)(std::string);

class Protocol {
   private:
    UniquePtr mainThread;
    // Pub Sub queue for inter protocol comms
    std::map<std::string, PubSub*> publishers;
    std::map<std::string, PubSub*> subscriptions;

    // Pointers to the publisher thread
    VectorThread pubThreads;
    // Pointers to the subscriber thread
    VectorThread subThreads;

    void _subscribe(std::string channel, PubSub* sub, SubCallback callback);

    // callback function to run thread instance
    static void exec(Protocol* pThread);

    // callback function to run thread instance
    static void subexec(PubSub* sub, SubCallback callback);

   public:
    Protocol();
    ~Protocol();

    void start();

    void join();

    void subscribe(Protocol* proto, std::string channel, SubCallback callback);

    void publish(std::string channel, char* buffer, int bufflen);

    void publish(std::string channel, std::string message);

    virtual void run() = 0;

   protected:
    std::string genFnName(std::string _class, std::string fn);
    std::string genError(std::string prefix, std::string msg);
    std::string genError(std::string prefix, std::string msg, int code);
};

#endif
