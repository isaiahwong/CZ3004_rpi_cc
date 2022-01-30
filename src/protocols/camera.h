#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>
#include <grpcpp/grpcpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "protocol.h"
#include "vision_client.h"

using Queue = moodycamel::ConcurrentQueue<cv::Mat>;

class Camera final : public Protocol {
   private:
    std::string name = "camera";
    std::string addr;

    VisionClient *visionClient;
    cv::VideoCapture *videoCap;

    Queue *q;

   public:
    Camera(std::string _addr);
    ~Camera();

    void onReadFrame();

    /**
     * @brief Forwarder static function to access camera
     *
     * @param c
     * @param msg
     */
    static void onCapture(void *c, std::string msg);

    void onCapture(std::string msg);

    void run();
};

#endif