#pragma once

#include "Services.h"
#include "UserConnectionView.h"

#include <fon/yaml.h>
#include <cndl/Route.h>
#include <cndl/Server.h>
#include <simplyfile/socket/Host.h>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <thread>

namespace webcom {
namespace details {


struct WebSocketHandler : cndl::WebsocketHandler {
    using UserData = std::unique_ptr<ViewBase>;

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
        auto view = services.subscribe("services", [&ws](YAML::Node node) {
            YAML::Emitter emit;
            emit << node;

            ws.send(std::string{emit.c_str()});
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
                auto node = YAML::Load(std::string{message});
                if (not node.IsMap()) {
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
    WebSocketHandler handler{*this};
    cndl::WSRoute<WebSocketHandler> wsroute;
    webcom::Controller<webcom::UserConnectionView> userController;


    CndlServices(cndl::Server& _cndlServer, std::string const& _resource)
        : wsroute   {std::regex{_resource}, handler}
    {
        addController("services", [&](webcom::Services::SendCB _send) {
           return userController.makeView(std::move(_send), *this);
        });

        _cndlServer.getDispatcher().addRoute(wsroute);
    }
};

}
using CndlServices = details::CndlServices;
}
