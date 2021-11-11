#pragma once

#include "UserConnection.h"
#include "Services.h"

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
    using UserData = UserConnection;

    using Websocket = cndl::Websocket;
    using Request   = cndl::Request;
    std::map<Websocket*, UserData> cndlUserData;
    Services& services;
    std::mutex& mutex;
    WebSocketHandler(Services& _services, std::mutex& _mutex)
        : services {_services}
        , mutex {_mutex}
    {}

    bool canOpen(Request const&) {
        fmt::print("request new connection\n");
        return true;
    }

    void onOpen([[maybe_unused]] Request const& request, Websocket& ws) {
        auto g = std::lock_guard(mutex);
        auto [iter, success] = cndlUserData.try_emplace(&ws);
        auto& userData = iter->second;
        userData.sendData = [&ws](std::string_view msg) {
            ws.send(msg);
        };
        userData.getBufferedAmount = [&ws]() {
            return ws.getOutBufferSize();
        };
        fmt::print("new connection\n");
    }

    void onMessage([[maybe_unused]] Websocket& ws, [[maybe_unused]] AnyMessage msg) override {
        auto g = std::lock_guard(mutex);
        auto& userData = cndlUserData[&ws];
        if (std::holds_alternative<std::string_view>(msg)) {
            auto const& message = std::get<std::string_view>(msg);
            try {
                auto node = YAML::Load(std::string{message});
                if (not node.IsMap()) {
                    throw std::runtime_error("invalid message");
                }
                auto serviceName = node["service"].as<std::string>();
                auto actionName  = node["action"].as<std::string>();
                auto params      = node["params"];

                if (serviceName == "services") {
                    if (actionName == "subscribe") {
                        auto serviceName = node["subscribeTo"].as<std::string>();
                        fmt::print("subscribe to {}\n", serviceName);

                        auto& service = services.getService(serviceName);
                        //userData.viewControllers.try_emplace(serviceName, ViewController{userData.sendData, userData.getBufferedAmount, service});
                        //auto& viewController = userData.viewControllers.at(serviceName);
                        //service.addViewController(viewController);
                        auto [iter, succ] = userData.viewControllers.try_emplace(serviceName, service.createViewController(userData.sendData, userData.getBufferedAmount));
                        iter->second->dispatchSignalFromClient("subscribe", params);
                    } else if (actionName == "unsubscribe") {
                        auto serviceName = node["unsubscribeFrom"].as<std::string>();
                        fmt::print("unsubscribe from {}\n", serviceName);

                        auto& service = services.getService(serviceName);
                        auto& viewController = userData.viewControllers.at(serviceName);

                        userData.viewControllers.erase(serviceName);
                    } else {
                        throw std::runtime_error(fmt::format("unknown action \"{}\"", actionName));
                    }
                } else {
                    auto& viewController = userData.viewControllers.at(serviceName);
                    viewController->dispatchSignalFromClient(actionName, params);
                }
            } catch(...) {
                fmt::print("exception when reading: \"{}\"", message);
                throw;
            }
        }
    }


    void onClose(Websocket& ws) override {
        auto g = std::lock_guard(mutex);
        auto& userData = cndlUserData[&ws];
        for (auto& [serviceName, viewController] : userData.viewControllers) {
            viewController->dispatchSignalFromClient("unsubscribe");
        }
        userData.viewControllers.clear();
        fmt::print("close connection\n");
        cndlUserData.erase(&ws);
    }

};

auto serveFile(std::filesystem::path file) -> cndl::OptResponse {
    auto ifs = std::ifstream(file.string(), std::ios::binary);
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    auto response = cndl::Response{buffer.str()};
    if (file.extension() == ".css") {
        response.fields.emplace("Content-Type", "text/css; charset=utf-8");
        return response;
    } else if (file.extension() == ".js") {
        response.fields.emplace("Content-Type", "text/javascript; charset=utf-8");
        return response;
    } else if (file.extension() == ".png") {
        response.fields.emplace("Content-Type", "image/png");
        return response;
    } else {
        response.fields.emplace("Content-Type", "text/html; charset=utf-8");
        return response;
    }
    std::cout << "not found " << file << "\n";
    return std::nullopt;
};

struct WebServices : Services {
    simplyfile::Epoll epoll;
    cndl::Server cndlServer{epoll};

    std::mutex mutex;

    using UserData = UserConnection;

    WebSocketHandler handler{*this, mutex};
    cndl::WSRoute<WebSocketHandler> wsroute{std::regex{R"(/ws)"}, handler};

    cndl::Route<cndl::OptResponse(cndl::Request const&)> defaultRoute;
    using Route = cndl::Route<cndl::OptResponse(cndl::Request const&, std::string_view)>;
    std::vector<Route> routes;

    WebServices(std::filesystem::path defaultFile, std::vector<std::tuple<std::string, std::filesystem::path>> paths)
    : defaultRoute {std::regex{R"(/)"}, [=](cndl::Request const&) -> cndl::OptResponse {
              return serveFile(defaultFile);
         }, {.methods={"GET"}}}
    {
        for (auto const& [url, path] : paths) {
            routes.emplace_back(std::regex{url}, [=](cndl::Request const&, std::string_view arg) -> cndl::OptResponse {
                fmt::print("serving: {}/{}\n", std::string{path},  arg);
                return serveFile(path / std::string{arg});
            }, Route::Options{.methods={"GET"}});
        }

        cndlServer.getDispatcher().addRoute(wsroute);
        cndlServer.getDispatcher().addRoute(defaultRoute);
        for (auto& r : routes) {
            cndlServer.getDispatcher().addRoute(r);
        }
    }
    auto listen(std::vector<simplyfile::Host> const& hosts) -> cndl::Server& {
        for (auto const& h : hosts) {
            cndlServer.listen(h);
        }
        return cndlServer;
    }
};

}
using WebServices = details::WebServices;
}
