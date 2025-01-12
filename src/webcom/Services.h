// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Controller.h"
#include "View.h"

#include <cassert>
#include <channel/value_mutex.h>
#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

class Services {
public:
    using SendCB = std::function<void(Json::Value)>;
    using CB     = std::function<std::unique_ptr<View>(SendCB)>;

private:
    channel::value_mutex<std::unordered_map<std::string, CB>> controllerList;

public:
    template <typename T>
        requires requires { typename T::View; }
    auto setController(std::string_view _key, T& object) {
        auto controller = std::make_shared<Controller<T&>>(object);
        auto _cb = [&, controller](SendCB _send) {
            return controller->template makeView<typename T::View>(std::move(_send));
        };

        auto [guard, list] = *controllerList;
        list->try_emplace(std::string{_key}, _cb);

        return controller;
    }

    template <typename Model, typename View = typename Model::View, typename ...Args>
    auto addController(std::string_view _key, Args&&... args) {
        auto controller = std::make_shared<Controller<Model>>(std::forward<Args>(args)...);
        auto _cb = [&, controller](SendCB _send) {
            return controller->template makeView<View>(std::move(_send));
        };

        auto [guard, list] = *controllerList;
        list->try_emplace(std::string{_key}, _cb);

        return controller;
    }



    auto subscribe(std::string_view _serviceName, SendCB _send) -> std::unique_ptr<View> {
        auto [guard, list] = *controllerList;

        auto iter = list->find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == list->end()) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second(std::move(_send));
    }
};

}
