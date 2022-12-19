#pragma once

#include "Controller.h"
#include "GuardedType.h"
#include "View.h"

#include <cassert>
#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

class Services {
public:
    using SendCB = std::function<void(Json::Value)>;
    using CB     = std::function<std::unique_ptr<ViewBase>(SendCB)>;

private:
    GuardedType<std::unordered_map<std::string, CB>> controllerList;

public:
    template <typename T, typename ...Args>
        requires requires { typename T::View; }
    auto setController(std::string_view _key, T& object, Args&&... args) {
        auto controller = std::make_shared<Controller<typename T::View>>();
        auto _cb = [&, controller](SendCB _send) {
            return controller->makeView(std::move(_send), object, std::forward<Args>(args)...);
        };

        auto&& [guard, list] = *controllerList;
        list.try_emplace(std::string{_key}, _cb);

        return controller;
    }

    auto subscribe(std::string_view _serviceName, SendCB _send) -> std::unique_ptr<ViewBase> {
        auto&& [guard, list] = *controllerList;

        auto iter = list.find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == end(list)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second(std::move(_send));
    }
};

}
