#pragma once

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <map>
#include <string>
#include <thread>
#include <vector>

#include "../common/pubsub.h"
#include "action.h"

using PubSub = PubSubQueue<1024>;
using UniqueThreadPtr = std::unique_ptr<std::thread>;
using VectorThread = std::vector<UniqueThreadPtr>;
class Protocol {
   public:
    /**
     * @brief Defines a void pointer to allow variable class as well as allowing
     * class to access member functions
     * https://stackoverflow.com/questions/12662891/how-can-i-pass-a-member-function-where-a-free-function-is-expected
     */
    typedef void (*SubCallback)(void* proto, std::string);

    /**
     * @brief void pointer to allow Actions
     *
     */
    typedef void (*SubActionCallback)(void* proto, Action*);

    /**
     * @brief void pointer to allow Actions
     *
     */
    typedef void (*SubResponseCallback)(void* proto, Response*);

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

    /**
     * @brief
     *
     * @param proto
     * @param channel
     * @param callback
     */
    void registerSub(Protocol* proto, std::string channel,
                     SubResponseCallback callback);

    /**
     * @brief Registers a protocol to be subscribed to a channel
     *
     * @param proto
     * @param channel
     * @param callback
     */
    void registerSub(Protocol* proto, std::string channel,
                     SubCallback callback);

    void registerSub(Protocol* proto, std::string channel,
                     SubActionCallback callback);

    // template<class T>
    // void publish(std::string channel, Action* a);

    void publish(std::string channel, Action& a);

    void publish(std::string channel, Response& a);

    void publish(std::string channel, char* buffer, int bufflen);

    void publish(std::string channel, std::string message);

    /**
     * @brief function to override
     *
     */
    virtual void run() = 0;

   protected:
    // Pointers to the sub threads within a protocol
    VectorThread subThreads;

    // Utility
    std::string genFnName(std::string _class, std::string fn);
    std::string genError(std::string prefix, std::string msg);
    std::string genError(std::string prefix, std::string msg, int code);

    void print(std::string msg);

    void printRed(std::string msg);

    /**
     * @brief no op loop to keep "main" protocol thread running
     *
     */
    void nooploop();

   private:
    UniqueThreadPtr mainThread;

    // Pub Sub queue for inter protocol comms
    std::map<std::string, PubSub*> pubsub;

    PubSub* getPub(std::string);

    void pushThread(std::thread*);

    void _registerSub(Protocol* proto, std::string channel, PubSub* sub,
                      SubResponseCallback callback);

    void _registerSub(Protocol* proto, std::string channel, PubSub* sub,
                      SubActionCallback callback);

    void _registerSub(Protocol* proto, std::string channel, PubSub* sub,
                      SubCallback callback);

    // callback function to run thread instance
    static void exec(Protocol* proto);

    static void subexec(Protocol* proto, PubSub* sub, SubCallback callback);

    // callback function to run thread instance
    static void subexecA(Protocol* proto, PubSub* sub,
                         SubActionCallback callback);

    // callback function to run thread instance
    static void subexecR(Protocol* proto, PubSub* sub,
                         SubResponseCallback callback);
};

#endif
