#include "camera.h"

#include <fmt/core.h>
#include <stdio.h>

#include <chrono>
#include <iostream>

Camera::Camera(std::string addr, int open, int width, int height) {
    this->addr = addr;
    this->width = width;
    this->height = height;
    this->visionClient = new VisionClient(addr);
    this->videoCap = new cv::VideoCapture();
    this->q = new Queue();
    this->open = open;
}

void Camera::run() {
    // Keep camera thread running
    open == 1 ? onReadFrame() : nooploop();
}

void Camera::openCamera() {
    int deviceID = 0;         // 0 = open default camera
    int apiID = cv::CAP_ANY;  // 0 = autodetect default API

    // Open selected camera using selected API
    this->videoCap->open(deviceID, apiID);
    this->videoCap->set(cv::CAP_PROP_FRAME_WIDTH, width);
    this->videoCap->set(cv::CAP_PROP_FRAME_HEIGHT, height);
    print("Camera: Camera opened");
}

/**
 * @brief Constantly reads frames from camera and puts it into queue..
 *
 */
void Camera::onReadFrame() {
    cv::Mat frame;
    cv::Mat noopFrame;

    openCamera();
    while (true) {
        // TODO handle error. Move to function
        if (!this->videoCap->isOpened()) {
            printRed("Camera not opened. Opening... in 500 ms");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            // Opens camera
            openCamera();
            continue;
        }

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
void Camera::onCapture(void* c, Action* action) {
    static_cast<Camera*>(c)->onCapture(action);
}

void Camera::onCapture(Action* action) {
    cv::Mat frame;

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
        VisionResponse* vres =
            visionClient->SendFrame(byteStr, width, height, channels);

        Response res(vres->imageid(), vres->name(), vres->status(),
                     vres->distance());
        // End Timer
        auto t3 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = t3 - t1;
        print(fmt::format("End Capture {} ms", ms.count()));
        print(fmt::format("ImageID: {}, Name: {}, Distance: {}", res.result,
                          res.name, res.distance));
        this->publish(Camera::CAM_CAPTURE_RESULT, res);
    } catch (std::string e) {
        printRed(e);
    }
}

Camera::~Camera() {
    this->videoCap->release();

    delete visionClient;
    delete videoCap;
}