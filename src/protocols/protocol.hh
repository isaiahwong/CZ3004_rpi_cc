#pragma once

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <string>
#include <thread>

#include "pubsub.hh"

using PubSub = PubSubQueue<1024>;

class Protocol {
   private:
    enum ThreadOp { READ, WRITE };

    // Pub Sub queue for inter protocol comms
    PubSub* pub;
    PubSub* sub;

    // Pointers to the thread
    std::unique_ptr<std::thread> readThread = nullptr;
    std::unique_ptr<std::thread> writeThread = nullptr;

    // callback function to run thread instance
    static void exec(Protocol* pThread, Protocol::ThreadOp op);

   public:
    Protocol();
    void setSub(PubSub* sub);
    void start();
    void join();
    void subscribe(Protocol* proto);
    void publish(char* buffer, int bufflen);
    void publish(std::string);

   protected:
    virtual void runRead() = 0;
    virtual void runWrite() = 0;
    std::string genFnName(std::string _class, std::string fn);
    std::string genError(std::string prefix, std::string msg);
    std::string genError(std::string prefix, std::string msg, int code);
};

#endif
