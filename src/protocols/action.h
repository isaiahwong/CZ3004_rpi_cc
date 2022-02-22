#pragma once

#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Response {
   public:
    std::string result;
    std::string coordinate;
    std::string type;
    int status;

    inline static const int OK = 1;
    inline static const int ERROR = 0;

    inline static std::string EMPTY = "____";

    ~Response(){};

    Response() {
        result = "";
        coordinate = "";
        type = "";
        status = 0;
    }

    Response(std::string type, std::string result, int status,
             std::string coordinate) {
        this->type = type;
        this->result = result;
        this->status = status;
        this->coordinate = coordinate;
    }

    Response(std::string result, int status) {
        this->result = result;
        this->status = status;
        this->coordinate = " ";
    }

    std::string emptyToFill(std::string s) { return s.empty() ? EMPTY : s; }

    std::string fillToEmpty(std::string s) {
        return s.compare(EMPTY) == 0 ? "" : s;
    }

    void serialiseEmpty() {
        type = emptyToFill(type);
        result = emptyToFill(result);
        coordinate = emptyToFill(coordinate);
    }

    void deserialiseEmpty() {
        result = fillToEmpty(result);
        coordinate = fillToEmpty(coordinate);
    }

    void to_json(json& j) {
        j = json{
            {"type", type},
            {"status", status},
            {"result", result},
            {"coordinate", coordinate},
        };
    }

    static void from_json(const json& j, Response& a) {
        j.at("type").get_to(a.type);
        j.at("status").get_to(a.status);
        j.at("result").get_to(a.result);
        j.at("coordinate").get_to(a.coordinate);
    }
};

class Action {
   public:
    using SharedPtrAction = std::shared_ptr<Action>;
    using VectorAction = std::vector<Action>;

    std::string type;
    std::string action;
    int angle;
    int length;
    std::string distance;
    std::string coordinate;

    inline static std::string EMPTY = "____";

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

    Action() {
        type = "";
        action = "";
        angle = 0;
        length = 0;
        distance = "";
        coordinate = "";
    }

    std::string emptyToFill(std::string s) { return s.empty() ? EMPTY : s; }

    std::string fillToEmpty(std::string s) {
        return s.compare(EMPTY) == 0 ? "" : s;
    }

    void serialiseEmpty() {
        type = emptyToFill(type);
        action = emptyToFill(action);
        distance = emptyToFill(distance);
        coordinate = emptyToFill(coordinate);
    }

    void deserialiseEmpty() {
        type = fillToEmpty(type);
        action = fillToEmpty(action);
        distance = fillToEmpty(distance);
        coordinate = fillToEmpty(coordinate);
    }

    /**
     * @brief Destroy the Action object
     *
     */
    ~Action();

    inline static const std::string TYPE_SERIES = "series";

    inline static const std::string TYPE_MOVE = "move";

    inline static const std::string TYPE_CAPTURE = "capture";

    void to_json(json& j, const Action& a);

    static void _from_json(const json& j, Action& a);

    static void from_json(const json& j, Action& a);
};

#endif