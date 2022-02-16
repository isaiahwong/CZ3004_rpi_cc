#include "action.h"

Action::~Action() {}

void Action::to_json(json& j, const Action& a) {
    j = json{
        {"type", a.type},
        {"action", a.action},
        {"angle", a.angle},
        {"distance", a.distance},
    };
}

void Action::_from_json(const json& j, Action& a) {
    j.at("type").get_to(a.type);
    j.at("action").get_to(a.action);
    j.at("angle").get_to(a.angle);
    j.at("distance").get_to(a.distance);
}

void Action::from_json(const json& j, Action& a) {
    _from_json(j, a);

    // Attempt to parse array data
    try {
        auto arr = j.at("data");

        for (auto it : arr) {
            Action* action = new Action();
            _from_json(it, *action);
            a.data.push_back(Action::SharedPtrAction(action));
        }
    } catch (...) {
    }
}