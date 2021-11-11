#pragma once

#include "Services.h"

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

struct UserViewController : ViewController {
    Services& services;
    std::unordered_map<std::string, std::unique_ptr<ViewController>> viewControllers;

    UserViewController(Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe", &UserViewController::subscribe);
        visitor("unsubscribe", &UserViewController::unsubscribe);
        visitor("message", &UserViewController::message);
    }

    void subscribe(std::string _serviceName) {
        fmt::print("subscribing to {}\n", _serviceName);
        viewControllers.try_emplace(_serviceName, services.subscribe(_serviceName, [this, _serviceName](YAML::Node _node) {
            _node["service"] = _serviceName;
            callBack("message")(_node);
        }));
    }

    void unsubscribe(std::string _serviceName) {
        viewControllers.erase(_serviceName);
    }

    void message(YAML::Node data) {
        std::cout << "received message\n";
        auto service = data["service"].as<std::string>();
        viewControllers.at(service)->dispatchSignalFromClient(data);
    }
};



struct WebSocketHandler : cndl::WebsocketHandler {
    using UserData = std::unique_ptr<ViewController>;

    using Websocket = cndl::Websocket;
    using Request   = cndl::Request;
    std::unordered_map<Websocket*, UserData> cndlUserData;
    Services& services;
    std::mutex& mutex;
    WebSocketHandler(Services& _services, std::mutex& _mutex)
        : services {_services}
        , mutex {_mutex}
    {}

    void onOpen([[maybe_unused]] Request const& request, Websocket& ws) {
        auto g = std::lock_guard(mutex);
        auto viewController = services.subscribe("services", [&ws](YAML::Node node) {
            YAML::Emitter emit;
            emit << node;

            ws.send(std::string{emit.c_str()});
        });
        cndlUserData.try_emplace(&ws, std::move(viewController));
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
                userData->dispatchSignalFromClient(node);
            } catch(...) {
                fmt::print("exception when reading: \"{}\"", message);
                throw;
            }
        }
    }


    void onClose(Websocket& ws) override {
        auto g = std::lock_guard(mutex);
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

    WebSocketHandler handler{*this, mutex};
    cndl::WSRoute<WebSocketHandler> wsroute{std::regex{R"(/ws)"}, handler};

    cndl::Route<cndl::OptResponse(cndl::Request const&)> defaultRoute;
    using Route = cndl::Route<cndl::OptResponse(cndl::Request const&, std::string_view)>;
    std::vector<Route> routes;

    Service& userService;
    WebServices(std::filesystem::path defaultFile, std::vector<std::tuple<std::string, std::filesystem::path>> paths)
    : defaultRoute {std::regex{R"(/)"}, [=](cndl::Request const&) -> cndl::OptResponse {
              return serveFile(defaultFile);
         }, {.methods={"GET"}}}
    , userService {provideViewController("services", [&]() {
            return webcom::make<UserViewController>(*this);
        })
    }
    {

        for (auto const& [url, path] : paths) {
            routes.emplace_back(std::regex{url}, [path=path](cndl::Request const&, std::string_view arg) -> cndl::OptResponse {
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
