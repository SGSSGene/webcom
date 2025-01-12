// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Services.h"
#include "View.h"

#include <fon/json.h>
#include <memory>
#include <unordered_map>

namespace webcom {

struct UserConnectionView : View<UserConnectionView> {
    Services& services;
    std::unordered_map<size_t, std::unique_ptr<ViewBase>> views;

    UserConnectionView(Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe",   &UserConnectionView::subscribe);
        visitor("unsubscribe", &UserConnectionView::unsubscribe);
        visitor("message",     &UserConnectionView::message);
    }

    void subscribe(size_t _id, std::string _serviceName) {
        views.try_emplace(_id, services.subscribe(std::move(_serviceName), [this, _id](Json::Value _node) {
            _node["id"] = _id;
            callBack("message")(_node);
        }));
    }

    void unsubscribe(size_t _id) {
        views.erase(_id);
    }

    void message(size_t id, Json::Value data) {
        auto& ptr = views.at(id);
        ptr->dispatchSignalFromClient(data);
    }
};

}
