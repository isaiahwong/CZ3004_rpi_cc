#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include "protocol.h"

#include <grpcpp/grpcpp.h>
#include "vision_client.h"

class Camera final : public Protocol {
    private: 
        std::string name = "camera";
        std::string addr;

        VisionClient *visionClient;


    public: 
    Camera(std::string _addr);
    ~Camera();

    /**
     * @brief Forwarder static function to access camera
     *
     * @param c
     * @param msg
     */
    static void onVideoOpen(void* c, std::string msg);

    void onVideoOpen(std::string msg);
    
    void run();
};

#endif