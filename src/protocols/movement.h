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
    inline static const std::string FORWARD_RIGHT_90 = "280";
    inline static const std::string BACK_RIGHT_90 = "250";
    inline static const std::string FORWARD_LEFT_90 = "062";
    inline static const std::string BACK_LEFT_90 = "078";

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

    typedef void (*WriteCallback)(void* o, std::string);

    void center(Action& a, void* o, WriteCallback fn) {
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

    void stop(Action& a, void* o, WriteCallback fn) {
        fn(o, fmt::format("{}{}000", CMD_STOP, CMD_STOP));
    }

    void forwardLeft(Action& a, void* o, WriteCallback fn) {
        auto cmd =
            fmt::format("{}{}{}", CMD_FORWARD, CMD_LEFT, FORWARD_LEFT_90);
        fn(o, cmd);
    }

    void forwardRight(Action& a, void* o, WriteCallback fn) {
        auto cmd =
            fmt::format("{}{}{}", CMD_FORWARD, CMD_RIGHT, FORWARD_RIGHT_90);
        fn(o, cmd);
    }

    void backLeft(Action& a, void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}{}", CMD_BACK, CMD_LEFT, BACK_LEFT_90);
        fn(o, cmd);
    }

    void backRight(Action& a, void* o, WriteCallback fn) {
        auto cmd = fmt::format("{}{}{}", CMD_BACK, CMD_RIGHT, BACK_RIGHT_90);
        fn(o, cmd);
    }
};

#endif