#pragma once

#include "Services.h"

#include <fon/yaml.h>
#include <memory>
#include <unordered_map>

namespace webcom {

template <typename T>
struct UserConnectionViewController : ViewController {
    Services<T>& services;
    T userData;
    std::unordered_map<size_t, std::unique_ptr<ViewController>> viewControllers;

    UserConnectionViewController(Services<T>& _services, T _userData)
        : services{_services}
        , userData{std::move(_userData)}
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
        }, userData));
    }

    void unsubscribe(size_t _id) {
        viewControllers.erase(_id);
    }

    void message(size_t id, YAML::Node data) {
        viewControllers.at(id)->dispatchSignalFromClient(data);
    }
};

}
