#pragma once

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <fmt/core.h>

#include <chrono>
#include <string>
#include <thread>

#include "action.h"

/**
 * @brief Movement class formatter for serial
 *
 */
class Movement {
   private:
    inline static const std::string DEFAULT_ANGLE = "000";
    // Predefined turning angles
    inline static const std::string _FORWARD_RIGHT_90 = "280";
    inline static const std::string _BACK_RIGHT_90 = "180";
    inline static const std::string _FORWARD_LEFT_90 = "065";
    inline static const std::string _BACK_LEFT_90 = "070";

    std::string FORWARD_RIGHT_90;
    std::string BACK_RIGHT_90;
    std::string FORWARD_LEFT_90;
    std::string BACK_LEFT_90;

   public:
    inline static const char CMD_FORWARD = 'F';
    inline static const char CMD_STOP = 'S';
    inline static const char CMD_CENTER = 'C';
    inline static const char CMD_BACK = 'B';
    inline static const char CMD_LEFT = 'L';
    inline static const char CMD_RIGHT = 'R';

    inline static const std::string FORWARD = "forward";
    inline static const std::string FORWARD_LEFT = "forward_left";
    inline static const std::string FORWARD_RIGHT = "forward_right";
    inline static const std::string STOP = "stop";
    inline static const std::string CENTER = "center";
    inline static const std::string BACK = "back";
    inline static const std::string BACK_LEFT = "back_left";
    inline static const std::string BACK_RIGHT = "back_right";

    Movement() {
        FORWARD_RIGHT_90 = _FORWARD_RIGHT_90;
        BACK_RIGHT_90 = _BACK_RIGHT_90;
        FORWARD_LEFT_90 = _FORWARD_LEFT_90;
        BACK_LEFT_90 = _BACK_LEFT_90;
    }

    Movement(std::string fr, std::string br, std::string fl, std::string bl) {
        FORWARD_RIGHT_90 = fr;
        BACK_RIGHT_90 = br;
        FORWARD_LEFT_90 = fl;
        BACK_LEFT_90 = bl;
    }

    typedef void (*WriteCallback)(void* o, std::string);

    void center(void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}000", CMD_CENTER, CMD_CENTER);
        fn(o, cmd);
    }

    void forward(Action& a, void* o, WriteCallback fn) {
        // FF = Forward
        auto cmd = fmt::format("{}{}{}", CMD_FORWARD, CMD_FORWARD, a.distance);
        fn(o, cmd);
    }

    void back(Action& a, void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}{}", CMD_BACK, CMD_BACK, a.distance);
        fn(o, cmd);
    }

    void stop(void* o, WriteCallback fn) {
        fn(o, fmt::format("{}{}000", CMD_STOP, CMD_STOP));
    }

    void forwardLeft(void* o, WriteCallback fn) {
        auto cmd =
            fmt::format("{}{}{}", CMD_FORWARD, CMD_LEFT, FORWARD_LEFT_90);
        fn(o, cmd);
    }

    void forwardRight(void* o, WriteCallback fn) {
        auto cmd =
            fmt::format("{}{}{}", CMD_FORWARD, CMD_RIGHT, FORWARD_RIGHT_90);
        fn(o, cmd);
    }

    void backLeft(void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}{}", CMD_BACK, CMD_LEFT, BACK_LEFT_90);
        fn(o, cmd);
    }

    void backRight(void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}{}", CMD_BACK, CMD_RIGHT, BACK_RIGHT_90);
        fn(o, cmd);
    }
};

#endif