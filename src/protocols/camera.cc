#include "camera.h"

#include <fmt/core.h>
#include <stdio.h>

#include <chrono>
#include <iostream>

Camera::Camera(std::string addr) {
    this->addr = addr;
    this->visionClient = new VisionClient(addr);
    this->videoCap = new cv::VideoCapture();
    this->q = new Queue();
}

void Camera::run() {
    int deviceID = 0;         // 0 = open default camera
    int apiID = cv::CAP_ANY;  // 0 = autodetect default API

    // Open selected camera using selected API
    this->videoCap->open(deviceID, apiID);

    // TODO handle error. Move to function
    if (!this->videoCap->isOpened()) {
        printRed("Unable to open camera");
        return;
    }

    // Keep camera thread running
    onReadFrame();
}

/**
 * @brief Constantly reads frames from camera and puts it into queue..
 *
 */
void Camera::onReadFrame() {
    cv::Mat frame;
    cv::Mat noopFrame;

    while (true) {
        this->videoCap->read(frame);

        // check if we succeeded
        if (frame.empty()) {
            printRed("Blank frame grabbed");
            continue;
        }

        // Discard old frame
        this->q->try_dequeue(noopFrame);
        // Insert new frame
        this->q->enqueue(frame);
    }
}

/**
 * @brief static function to forward
 *
 * @param c
 * @param msg
 */
void Camera::onCapture(void* c, std::string msg) {
    static_cast<Camera*>(c)->onCapture(msg);
}

void Camera::onCapture(std::string msg) {
    cv::Mat frame;

    print(msg);

    // Start time
    print("\nStart Capture");
    auto t1 = std::chrono::high_resolution_clock::now();

    // Retrieves photo frames from queue
    bool found = this->q->try_dequeue(frame);

    // check if we succeeded
    if (!found) {
        printRed("No frame grabbed");
        return;
    }

    cv::Size matSize = frame.size();
    int width = matSize.width, height = matSize.height,
        channels = frame.channels();

    // Convert cv mat to byte string
    std::string byteStr(frame.datastart, frame.dataend);
    try {
        Response* res =
            visionClient->SendFrame(byteStr, width, height, channels);
        print(fmt::format("{}", res->count()));
    } catch (std::string e) {
        printRed(e);
    }
    // End Timer
    auto t3 = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = t3 - t1;
    print(fmt::format("End Capture {} ms", ms.count()));
}

Camera::~Camera() {
    this->videoCap->release();

    delete visionClient;
    delete videoCap;
}