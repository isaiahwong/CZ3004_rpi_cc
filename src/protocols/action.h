#pragma once

#ifndef ACTION_H
#define ACTION_H
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Response {
   public:
    std::string result;
    int status;

    inline static const int OK = 1;
    inline static const int ERROR = 0;

    Response(std::string result, int status) {
        this->result = result;
        this->status = status;
    }

    void to_json(json& j) {
        j = json{
            {"status", status},
            {"result", result},

        };
    }

    static void from_json(const json& j, Response& a) {
        j.at("status").get_to(a.status);
        j.at("result").get_to(a.result);
    }
};

class Action {
   public:
    std::string type;
    std::string action;
    int angle;
    std::string distance;

    inline static const std::string TYPE_MOVE = "move";

    inline static const std::string TYPE_CAPTURE = "capture";

    void to_json(json& j, const Action& a) {
        j = json{
            {"type", a.type},
            {"action", a.action},
            {"angle", a.angle},
            {"distance", a.distance},
        };
    }

    static void from_json(const json& j, Action& a) {
        j.at("type").get_to(a.type);
        j.at("action").get_to(a.action);
        j.at("angle").get_to(a.angle);
        j.at("distance").get_to(a.distance);
        std::cout << j.at("data").size() << std::endl;
    }
};

#endif