#pragma once

#include "Services.h"

#include <fon/yaml.h>
#include <memory>
#include <unordered_map>

namespace webcom {

struct UserConnectionViewController : ViewController {
    Services& services;
    std::unordered_map<std::string, std::unique_ptr<ViewController>> viewControllers;

    UserConnectionViewController(Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe",   &UserConnectionViewController::subscribe);
        visitor("unsubscribe", &UserConnectionViewController::unsubscribe);
        visitor("message",     &UserConnectionViewController::message);
    }

    void subscribe(std::string _serviceName) {
        viewControllers.try_emplace(_serviceName, services.subscribe(_serviceName, [this, _serviceName](YAML::Node _node) {
            _node["service"] = _serviceName;
            callBack("message")(_node);
        }));
    }

    void unsubscribe(std::string _serviceName) {
        viewControllers.erase(_serviceName);
    }

    void message(YAML::Node data) {
        auto service = data["service"].as<std::string>();
        viewControllers.at(service)->dispatchSignalFromClient(data);
    }
};

}
