
#pragma once
#ifndef HTTTP_H_

#define HTTTP_H_
#include "../../external/http/httplib.h"
#include "action.h"
#include "protocol.h"

class Htttp final : public Protocol {
   public:
    inline static const std::string HTTTP_ACTION = "HTTTP_ACTION";
    inline static const std::string HTTTP_SERIES_ACTIONS =
        "HTTTP_SERIES_ACTIONS";
    inline static const std::string HTTTP_SERIES_INTERLEAVE =
        "HTTTP_SERIES_INTERLEAVE";
    inline static const std::string HTTTP_CMD_RESET = "HTTTP_CMD_RESET";

    void run() {
        // HTTP
        httplib::Server svr;

        svr.Post(
            "/", [this](const httplib::Request &req, httplib::Response &res) {
                std::cout << req.body << std::endl;

                Action a;

                try {
                    // parse string to json
                    json data = json::parse(req.body);
                    // deserialise json to Action
                    Action::from_json(data, a);
                } catch (const json::parse_error &e) {
                    printRed("JSON error:");
                    // Issues with fmt::format
                    std::cout << e.what() << std::endl;
                    return;
                } catch (...) {
                    //  reset series if an exception is caught
                    printRed("TTT Parse: Unexpected error occurred");
                    return;
                }

                // map to different channels
                if (a.type.compare(Action::TYPE_MOVE) == 0)
                    this->publish(Htttp::HTTTP_ACTION, a);
                else if (a.type.compare(Action::TYPE_CAPTURE) == 0)
                    this->publish(Htttp::HTTTP_ACTION, a);
                else if (a.type.compare(Action::TYPE_SERIES) == 0)
                    this->publish(Htttp::HTTTP_SERIES_ACTIONS, a);
                else if (a.type.compare(Action::TYPE_SERIES_INTERLEAVE) == 0)
                    this->publish(Htttp::HTTTP_SERIES_INTERLEAVE, a);
                else if (a.type.compare(Action::TYPE_RESET) == 0)
                    this->publish(Htttp::HTTTP_CMD_RESET, a);

                // Send okay
                res.set_content("{ \"status\": \"ok\" }", "application/json");
            });

        svr.listen("0.0.0.0", 8080);
    }
};

#endif