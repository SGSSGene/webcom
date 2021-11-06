#pragma once

#include <App.h>

#include "UserConnection.h"
#include "Services.h"

#include <fstream>
#include <filesystem>

namespace webcom {
namespace details {

struct WebServices : Services {
    uWS::App app;

    std::mutex mutex;

    using UserData = UserConnection;

    WebServices(std::string defaultFile, std::vector<std::tuple<std::string, std::string>> paths) {
        auto serveFile = [](std::filesystem::path file, auto* res) {
            auto ifs = std::ifstream(file.string(), std::ios::binary);
            std::stringstream buffer;
            buffer << ifs.rdbuf();
            if (file.extension() == ".css") {
                res->writeHeader("Content-Type", "text/css; charset=utf-8")->end(buffer.str());
            } else if (file.extension() == ".js") {
                res->writeHeader("Content-Type", "text/javascript; charset=utf-8")->end(buffer.str());
            } else {
                res->writeHeader("Content-Type", "text/html; charset=utf-8")->end(buffer.str());
            }
            fmt::print("url request: {}\n", file.string());
        };

        static auto _try = [](auto ws, auto cb) {
            try {
                cb();
            } catch(std::exception const& e) {
                fmt::print("exception: {}\n", e.what());
            } catch(...) {
                fmt::print("exception: unknown exception, closing socket\n");
                ws->close();
            }
        };


        // setup - websockets
        app
        .get("/", [&, defaultFile](auto* res, auto* req) {
            auto g = std::lock_guard(mutex);
            serveFile(defaultFile, res);
        });

        for (auto const& [key, path] : paths) {
            app.get(key, [&, key, path](auto* res, auto* req) {
                auto g = std::lock_guard(mutex);
                auto rootPath = std::filesystem::path{path};
                auto file     = relative(std::filesystem::path{req->getUrl()}, "/");
                serveFile(rootPath / file, res);
            });
        }
        app.ws<UserData>("/ws", {
            .compression = uWS::DISABLED,
            .maxPayloadLength = 256 * 1024,
            .idleTimeout = 120,
            .maxBackpressure = 1 * 1024 * 1204,
            .open = [&](auto* ws, auto* req) {
                auto g = std::lock_guard(mutex);
                _try(ws, [&]() {
                    (void)req;
                    auto& userData = *std::launder(static_cast<UserData*>(ws->getUserData()));
                    userData.sendData = [ws](YAML::Node node) {
                        YAML::Emitter emit;
                        emit << node;
                        std::stringstream ss;
                        ss << emit.c_str();
                        ws->send(ss.str());
                    };
                    fmt::print("new connection\n");
                });
            },
            .message = [&](auto* ws, std::string_view message, uWS::OpCode opCode) {
                auto g = std::lock_guard(mutex);
                (void)opCode;
                auto& userData = *static_cast<UserData*>(ws->getUserData());
                _try(ws, [&]() {
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

                                auto& service = getService(serviceName);
                                userData.adapters.try_emplace(serviceName, Adapter{userData.sendData, service});
                                auto& adapter = userData.adapters.at(serviceName);
                                service.addAdapter(adapter);
                                service.dispatchSignalFromClient("subscribe", adapter, params);
                            } else if (actionName == "unsubscribe") {
                                auto serviceName = node["unsubscribeFrom"].as<std::string>();
                                fmt::print("unsubscribe from {}\n", serviceName);

                                auto& service = getService(serviceName);
                                auto& adapter = userData.adapters.at(serviceName);

                                service.removeAdapter(adapter);
                                userData.adapters.erase(serviceName);
                            } else {
                                throw std::runtime_error(fmt::format("unknown action \"{}\"", actionName));
                            }
                        } else {
                            auto& adapter = userData.adapters.at(serviceName);
                            adapter.service.dispatchSignalFromClient(actionName, adapter, params);
                        }
                    } catch(...) {
                        fmt::print("exception when reading: \"{}\"", message);
                        throw;
                    }
                });
            },
            .close = [&](auto* ws, int code, std::string_view message) {
                auto g = std::lock_guard(mutex);
                _try(ws, [&]() {
                    auto& userData = *static_cast<UserData*>(ws->getUserData());
                    for (auto& [serviceName, adapter] : userData.adapters) {
                        adapter.service.dispatchSignalFromClient("unsubscribe", adapter, YAML::Node{});
                        auto& service = getService(serviceName);
                        service.removeAdapter(adapter);
                    }
                    userData.adapters.clear();
                    fmt::print("close connection\n");
                });
            }
        });

    }
    void run() {
        app.listen(9001, [](auto* token) {});
        app.run();
    }
};

}
using WebServices = details::WebServices;
}
