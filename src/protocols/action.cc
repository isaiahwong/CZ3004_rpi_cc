#include "action.h"

Action::~Action() {}

void Action::to_json(json& j, const Action& a) {
    j = json{
        {"type", a.type},
        {"action", a.action},
        {"angle", a.angle},
        {"distance", a.distance},
        {"delay", a.delay},
        {"coordinate", a.coordinate},
        {"prev_coordinate", a.prev_coordinate},
        {"length", a.length},
    };
}

void Action::_from_json(const json& j, Action& a) {
    j.at("type").get_to(a.type);
    j.at("action").get_to(a.action);
    j.at("angle").get_to(a.angle);
    j.at("delay").get_to(a.delay);
    j.at("distance").get_to(a.distance);
    j.at("coordinate").get_to(a.coordinate);
    j.at("prev_coordinate").get_to(a.prev_coordinate);
    j.at("length").get_to(a.length);
}

void Action::from_json(const json& j, Action& a) {
    _from_json(j, a);

    // Attempt to parse array data
    try {
        auto arr = j.at("data");

        for (auto it : arr) {
            Action subaction;
            _from_json(it, subaction);
            a.data.push_back(subaction);
            // a.data.push_back(Action::SharedPtrAction(action));
        }
    } catch (const std::exception& exc) {
        std::cout << exc.what() << std::endl;
    }
}