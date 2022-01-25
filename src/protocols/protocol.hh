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

class Protocol {
   public:
    /**
     * @brief Defines a void pointer to allow variable class as well as allowing
     * class to access member functions
     * https://stackoverflow.com/questions/12662891/how-can-i-pass-a-member-function-where-a-free-function-is-expected
     */
    typedef void (*SubCallback)(void* proto, std::string);

    /**
     * @brief Construct a new Protocol object
     *
     */
    Protocol();

    /**
     * @brief Destroy the Protocol
     *
     */
    ~Protocol();

    /**
     * @brief Starts protocol main thread
     *
     */
    void start();

    /**
     * @brief Completes thread.
     *
     */
    void join();

    void subscribe(Protocol* proto, std::string channel, SubCallback callback);

    void publish(std::string channel, char* buffer, int bufflen);

    void publish(std::string channel, std::string message);

    virtual void run() = 0;

   protected:
    std::string genFnName(std::string _class, std::string fn);
    std::string genError(std::string prefix, std::string msg);
    std::string genError(std::string prefix, std::string msg, int code);

   private:
    UniquePtr mainThread;
    // Pub Sub queue for inter protocol comms
    std::map<std::string, PubSub*> publishers;
    std::map<std::string, PubSub*> subscriptions;

    // Pointers to the publisher thread
    VectorThread pubThreads;
    // Pointers to the subscription threads
    VectorThread subThreads;

    void _subscribe(Protocol* proto, std::string channel, PubSub* sub,
                    SubCallback callback);

    // callback function to run thread instance
    static void exec(Protocol* proto);

    // callback function to run thread instance
    static void subexec(Protocol* proto, PubSub* sub, SubCallback callback);
};

#endif
