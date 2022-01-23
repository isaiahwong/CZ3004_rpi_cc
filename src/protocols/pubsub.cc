// /**
//  * @file pubsub.cc
//  * @author your name (you@domain.com)
//  * @brief
//  * @version 0.1
//  * @date 2022-01-23
//  *
//  * @copyright Copyright (c) 2022
//  * Adapted from https://gist.github.com/film42/0cef2472e6ac75dee2a7
//  */
// #include <queue>
// #include <string>

// template <class T>
// class Pub {
//    private:
//     bool _exists(std::string channel) {
//         return (m_channels.count(channel) > 0) ? true : false;
//     }

//     std::map<std::string, FuncVector> m_channels;

//    public:
//     typedef std::vector<std::function<void(T)>> FuncVector;

//     void subscribe(std::string channel, std::function<void(T)> func) {
//         if (_exists(channel)) {
//             m_channels[channel].push_back(func);
//         } else {
//             FuncVector func_vec = {func};
//             auto pair = std::make_pair(channel, func_vec);
//             m_channels.insert(pair);
//         }
//     }

//     void publish(std::vector<std::string> channels, T message) {
//         for (auto chan : channels) {
//             publish(chan, message);
//         }
//     }

//     void publish(std::string channel, T message) {
//         if (_exists(channel)) {
//             for (auto& func : m_channels[channel]) {
//                 func(message);
//             }
//         }
//     }
// };
