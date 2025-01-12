// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Services.h"
#include "UserConnectionView.h"

#include <cndl/Route.h>
#include <cndl/Server.h>
#include <filesystem>
#include <fon/yaml.h>
#include <fstream>
#include <iostream>
#include <json/reader.h>
#include <simplyfile/socket/Host.h>
#include <thread>

namespace webcom {
namespace details {


struct WebSocketHandler : cndl::WebsocketHandler {
    using UserData = std::unique_ptr<View>;

    using Websocket = cndl::Websocket;
    using Request   = cndl::Request;
    std::unordered_map<Websocket*, UserData> cndlUserData;
    Services& services;
    std::mutex mutex;

    WebSocketHandler(Services& _services)
        : services {_services}
    {}

    void onOpen([[maybe_unused]] Request const& request, Websocket& ws) {
        auto g = std::lock_guard{mutex};
        auto view = services.subscribe("services", [&ws](Json::Value node) {
            ws.send(Json::FastWriter{}.write(node));
        });
        cndlUserData.try_emplace(&ws, std::move(view));
        fmt::print("new connection\n");
    }

    void onMessage([[maybe_unused]] Websocket& ws, [[maybe_unused]] AnyMessage msg) override {
        auto g = std::lock_guard{mutex};
        auto& userData = cndlUserData[&ws];
        if (std::holds_alternative<std::string_view>(msg)) {
            auto const& message = std::get<std::string_view>(msg);
            try {
                Json::Value node;
                Json::Reader{}.parse(std::string{message}, node);
                if (not node.isObject()) {
                    throw std::runtime_error("invalid message");
                }
                userData->dispatchSignalFromClient(node);
            } catch(...) {
                fmt::print("exception when reading: \"{}\"", message);
                throw;
            }
        }
    }


    void onClose(Websocket& ws) override {
        auto g = std::lock_guard{mutex};
        fmt::print("close connection\n");
        cndlUserData.erase(&ws);
    }

};


struct CndlServices : Services {
    using View = webcom::UserConnectionView;

    WebSocketHandler handler{*this};
    cndl::WSRoute<WebSocketHandler> wsroute;

    CndlServices(cndl::Server& _cndlServer, std::string const& _resource)
        : wsroute   {std::regex{_resource}, handler}
    {
        setController("services", *this);
        _cndlServer.getDispatcher().addRoute(wsroute);
    }
};

}
using CndlServices = details::CndlServices;
}
