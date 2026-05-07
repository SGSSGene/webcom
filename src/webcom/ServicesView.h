// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Services.h"
#include "View.h"

#include <fon/json.h>
#include <memory>
#include <unordered_map>

namespace webcom {

struct ServicesView : View {
    Services& services;
    std::unordered_map<size_t, std::unique_ptr<View>> views;

    ServicesView(Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe",     &ServicesView::subscribe);
        visitor("unsubscribe",   &ServicesView::unsubscribe);
        visitor("msgFromClient", &ServicesView::message);
        visitor("ping",          &ServicesView::ping);
    }

    void subscribe(size_t _id, std::string _serviceName) {
        views.try_emplace(_id, services.subscribe(std::move(_serviceName), [this, _id](Json::Value _node) {
            Json::Value parent;
            callBack("msgToClient", _id, _node);
        }));
    }

    void unsubscribe(size_t _id) {
        views.erase(_id);
    }

    void message(size_t id, Json::Value data) {
        auto& ptr = views.at(id);
        ptr->dispatchSignalFromClient(data);
    }

    void ping() {
        callBack("pong");
    }
};

}
