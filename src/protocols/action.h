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
    using SharedPtrAction = std::shared_ptr<Action>;
    using VectorAction = std::vector<SharedPtrAction>;

    std::string type;
    std::string action;
    int angle;
    std::string distance;

    /**
     * @brief check if action should read list of data
     *
     */
    bool chunk;

    /**
     * @brief List of actions
     *
     */
    VectorAction data;

    Action() {}

    /**
     * @brief Destroy the Action object
     *
     */
    ~Action();

    inline static const std::string TYPE_MOVE = "move";

    inline static const std::string TYPE_CAPTURE = "capture";

    void to_json(json& j, const Action& a);

    static void _from_json(const json& j, Action& a);

    static void from_json(const json& j, Action& a);
};

#endif