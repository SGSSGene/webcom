#pragma once

#include "Services.h"

#include <fon/yaml.h>
#include <memory>
#include <unordered_map>

namespace webcom {

struct UserConnectionViewController : ViewController {
    Services& services;
    std::unordered_map<size_t, std::unique_ptr<ViewController>> viewControllers;

    UserConnectionViewController(Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe",   &UserConnectionViewController::subscribe);
        visitor("unsubscribe", &UserConnectionViewController::unsubscribe);
        visitor("message",     &UserConnectionViewController::message);
    }

    void subscribe(size_t _id, std::string _serviceName) {
        viewControllers.try_emplace(_id, services.subscribe(std::move(_serviceName), [this, _id](YAML::Node _node) {
            _node["id"] = _id;
            callBack("message")(_node);
        }));
    }

    void unsubscribe(size_t _id) {
        viewControllers.erase(_id);
    }

    void message(size_t id, YAML::Node data) {
        viewControllers.at(id)->dispatchSignalFromClient(data);
    }
};

}
