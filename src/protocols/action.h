#pragma once

#ifndef ACTION_H
#define ACTION_H

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Action {
   public:
    std::string type;
    std::string action;
    int angle;
    int distance;

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
    }
};

#endif