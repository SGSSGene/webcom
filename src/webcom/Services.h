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

struct ServicesView;

class Services {
public:
    using View   = ServicesView;
    using SendCB = std::function<void(Json::Value)>;
    using CB     = std::function<std::unique_ptr<webcom::View>(SendCB)>;

private:
    Controller<Services&> controller{*this};

    // list of controll objects and a function to create a new view onto them
    channel::value_mutex<std::unordered_map<std::string, ControllerBase*>> controllerList;

public:
    void removeController(std::string_view _key) {
        controllerList->erase(std::string{_key});
    }

    // register a controller with a external lifetime
    template <typename T, typename TView>
    void registerController(std::string_view _key, webcom::Controller<T, TView>& controller) {
        auto [guard, list] = *controllerList;
        list->try_emplace(std::string{_key}, &controller);
    }

    Services() {
        registerController("services", controller);
    }

    auto subscribe(std::string_view _serviceName, SendCB _send) -> std::unique_ptr<webcom::View> {
        auto [guard, list] = *controllerList;

        auto iter = list->find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == list->end()) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second->makeView(std::move(_send));
    }
};

}

#include "ServicesView.h"
