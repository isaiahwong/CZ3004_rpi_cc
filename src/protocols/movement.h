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
    inline static const std::string RIGHT_90 = "057";
    inline static const std::string RIGHT_180 = "114";
    inline static const std::string LEFT_90 = "028";
    inline static const std::string LEFT_180 = "059";

    std::string getAngleLeft(Action& a) {
        std::string angle = DEFAULT_ANGLE;
        if (a.angle == 90)
            angle = LEFT_90;
        else if (a.angle == 180)
            angle = LEFT_180;
        return angle;
    }

    std::string getAngleRight(Action& a) {
        std::string angle = DEFAULT_ANGLE;
        if (a.angle == 90)
            angle = RIGHT_90;
        else if (a.angle == 180)
            angle = RIGHT_180;
        return angle;
    }

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

    void left(Action& a, char direction, void* o, WriteCallback fn) {
        // Turn car wheel left
        std::string angle = getAngleLeft(a);
        auto cmd = fmt::format("{}{}{}", direction, CMD_LEFT, angle);
        fn(o, cmd);
    }

    void right(Action& a, char direction, void* o, WriteCallback fn) {
        // Turn car wheel right
        std::string angle = getAngleRight(a);
        auto cmd = fmt::format("{}{}{}", direction, CMD_RIGHT, angle);
        fn(o, cmd);
    }

    void forwardLeft(Action& a, void* o, WriteCallback fn) {
        left(a, CMD_FORWARD, o, fn);
    }

    void forwardRight(Action& a, void* o, WriteCallback fn) {
        right(a, CMD_FORWARD, o, fn);
    }

    void backLeft(Action& a, void* o, WriteCallback fn) {
        left(a, CMD_BACK, o, fn);
    }

    void backRight(Action& a, void* o, WriteCallback fn) {
        right(a, CMD_BACK, o, fn);
    }
};

#endif