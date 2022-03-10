#pragma once
#ifndef CAMERA_H_
#define CAMERA_H_

#include <blockingconcurrentqueue.h>
#include <concurrentqueue.h>
#include <grpcpp/grpcpp.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "action.h"
#include "protocol.h"
#include "vision_client.h"

using Queue = moodycamel::ConcurrentQueue<cv::Mat>;

class Camera final : public Protocol {
   private:
    std::string name = "camera";
    std::string addr;
    int width;
    int height;

    int open;

    VisionClient *visionClient;
    cv::VideoCapture *videoCap;

    Queue *q;

    /**
     * @brief Opens the front camera of rpi
     *
     */
    void openCamera();

    void onReadFrame();

   public:
    // Channels
    inline static const std::string CAM_CAPTURE_RESULT = "CAM_CAPTURE_RESULT";

    Camera(std::string _addr, int open, int width, int height);
    ~Camera();

    /**
     * @brief Forwarder static function to access camera
     *
     * @param c
     * @param msg
     */
    static void onCapture(void *c, Action *action);
    void onCapture(Action *action);

    /**
     * @brief Runs Camera main functionality
     *
     */
    void run();
};

#endif