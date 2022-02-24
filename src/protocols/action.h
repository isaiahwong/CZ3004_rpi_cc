#pragma once

#ifndef ACTION_H
#define ACTION_H

#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Response {
   public:
    std::string result;
    std::string name;
    std::string coordinate;
    std::string type;
    int distance;
    int status;

    inline static const int OK = 1;
    inline static const int ERROR = 0;

    inline static std::string EMPTY = "____";

    ~Response(){};

    Response() {
        result = "";
        name = "";
        coordinate = "";
        type = "";
        status = 0;
        distance = 0;
    }

    Response(std::string type, std::string result, std::string name, int status,
             std::string coordinate, int distance) {
        this->type = type;
        this->result = result;
        this->name = name;
        this->status = status;
        this->coordinate = coordinate;
        this->distance = distance;
    }

    Response(int status) {
        this->status = status;
        this->result = "";
        this->name = "";
        this->coordinate = "";
        this->name = "";
        this->distance = 0;
    }

    Response(std::string result, std::string name, int status, int distance) {
        this->result = result;
        this->name = name;
        this->status = status;
        this->coordinate = "";
        this->name = "";
        this->distance = distance;
    }

    std::string emptyToFill(std::string s) { return s.empty() ? EMPTY : s; }

    std::string fillToEmpty(std::string s) {
        return s.compare(EMPTY) == 0 ? "" : s;
    }

    void serialiseEmpty() {
        type = emptyToFill(type);
        name = emptyToFill(name);
        result = emptyToFill(result);
        coordinate = emptyToFill(coordinate);
    }

    void deserialiseEmpty() {
        type = fillToEmpty(type);
        result = fillToEmpty(result);
        name = fillToEmpty(name);
        coordinate = fillToEmpty(coordinate);
    }

    void to_json(json& j) {
        j = json{
            {"type", type}, {"status", status},         {"result", result},
            {"name", name}, {"coordinate", coordinate}, {"distance", distance}};
    }

    static void from_json(const json& j, Response& a) {
        j.at("type").get_to(a.type);
        j.at("status").get_to(a.status);
        j.at("result").get_to(a.result);
        j.at("name").get_to(a.name);
        j.at("coordinate").get_to(a.coordinate);
        j.at("distance").get_to(a.distance);
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

    inline static const std::string TYPE_BULLSEYE = "bullseye";

    inline static const std::string TYPE_SERIES = "series";

    inline static const std::string TYPE_MOVE = "move";

    inline static const std::string TYPE_CAPTURE = "capture";

    void to_json(json& j, const Action& a);

    static void _from_json(const json& j, Action& a);

    static void from_json(const json& j, Action& a);
};

#endif