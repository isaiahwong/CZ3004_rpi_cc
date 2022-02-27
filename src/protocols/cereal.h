#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include <blockingconcurrentqueue.h>
#include <grpcpp/grpcpp.h>

#include <string>
#include <vector>

#include "action.h"
#include "iostream"
#include "movement.h"
#include "protocol.h"
#include "vision.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::ClientReader;
using grpc::Status;

using BlockingQueueAction = moodycamel::BlockingConcurrentQueue<Action>;
using BlockingQueueStatus = moodycamel::BlockingConcurrentQueue<int>;

class Cereal : public Protocol {
   private:
    std::unique_ptr<SerialService::Stub> stub_;
    std::unique_ptr<ClientReader<SerialResponse>> reader;

    std::string port;

    int serial = -1;

    int baudrate;

    int host_range = 20;

    /**
     * @brief Movement helper class for STM movements
     *
     */
    Movement movement;

    /**
     * @brief Blocking queue of commands
     *
     */
    // BlockingQueueAction commands;

    // BlockingQueueStatus statuses;

    std::atomic<bool> write;

    /**
     * @brief
     *
     */
    std::vector<std::string> hosts;

    int hostPointer = 0;

    void runWrite();

    void init();

    void addHost(std::string _port);

    int connect();

   public:
    // Channels
    inline static const std::string SERIAL_MAIN_READ = "SERIAL_MAIN_READ";
    inline static const std::string SERIAL_MAIN_WRITE_SUCCESS =
        "SERIAL_MAIN_WRITE_SUCCESS";

    Cereal(std::string _port, int _baudrate);

    Cereal(std::string _port, int _baudrate, std::string fr, std::string br,
           std::string fl, std::string bl);

    ~Cereal();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onConnectionRead(void* c);

    void onConnectionRead();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onExecuteActions(void* c);

    void onExecuteActions();

    /**
     * @brief Function to determine if write to stm is allowed
     *
     * @return true
     * @return false
     */
    bool canWrite();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void onAction(void* c, Action* action);

    void onAction(Action* action);

    // void onSeriesActions(Action* action);

    void run();

    void readClient();

    /**
     * @brief Forwarder static function to access Cereal onAction
     *
     * @param c
     * @param msg
     */
    static void writeClient(void* c, std::string msg);

    void writeClient(std::string msg);
};

#endif