#include <stdio.h>

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>

#include "camera.h"

using namespace cv;

Camera::Camera(std::string addr) {
    this->addr = addr;
    this->visionClient = new VisionClient(addr);

}

void Camera::run() {
    // Keep camera thread running
    while (true) {}
}

/**
 * @brief static function to forward
 *
 * @param c
 * @param msg
 */
void Camera::onVideoOpen(void* c, std::string msg) {
    static_cast<Camera*>(c)->onVideoOpen(msg);
}

void Camera::onVideoOpen(std::string msg) {
    Mat frame;
    VideoCapture cap;
    int deviceID = 0;         // 0 = open default camera
    int apiID = cv::CAP_ANY;  // 0 = autodetect default API

    // Open selected camera using selected API
    cap.open(deviceID, apiID);

    if (!cap.isOpened()) {
        // TODO: error handling
        printRed("Unable to open camera");
        return;
    }

    cap.read(frame);
    // check if we succeeded
    if (frame.empty()) {
        printRed("Blank frame grabbed");
        // break;
    }

    Size matSize = frame.size();
    int width  = matSize.width,
        height = matSize.height,
        channels =  frame.channels();
    
    std::string byteStr(frame.datastart, frame.dataend);
    visionClient->SendFrame(byteStr, width, height, channels);
    
    // show live and wait for a key with timeout long enough to show images
    //    imwrite(msg+"_test.jpg", frame);
    // while (true) {
    //     // wait for a new frame from camera and store it into 'frame'
    //     cap.read(frame);
    //     // check if we succeeded
    //     if (frame.empty()) {
    //         printRed(ERROR! blank frame grabbed);
    //         break;
    //     }
    //     // show live and wait for a key with timeout long enough to show images
    //     imshow("Live", frame);
    //     if (waitKey(5) >= 0) break;
    // }
    // the camera will be deinitialized automatically in VideoCapture destructor
}


Camera::~Camera() {
    delete visionClient;
}