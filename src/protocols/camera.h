#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "protocol.h"

#include <grpcpp/grpcpp.h>


class Camera final : public Protocol {
    private: 
        std::string name = "blueteeth";

    public: 
    /**
     * @brief Forwarder static function to access camera
     *
     * @param c
     * @param msg
     */
    static void onVideoOpen(void* c, std::string msg);

    void onVideoOpen(std::string msg);
    ~Camera();
    void run();
};

#endif