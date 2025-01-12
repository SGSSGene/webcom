// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Controller.h"

namespace webcom {

struct View {
    using SendData = std::function<void(Json::Value)>;
    thread_local static inline SendData gSendData;
    SendData sendData{std::move(gSendData)};

    std::function<void(Json::Value)> dispatchSignalFromClient;

    std::function<void()> cleanup;
    std::function<channel::value_mutex<std::unordered_set<View*>> const&()> getViews;

    std::unordered_map<std::string, std::function<void(Json::Value)>> callablesFromClient;

    View() = default;
    View(View const&) = delete;
    View(View&&) = delete;

    virtual ~View() {
        cleanup();
    }

    auto operator=(View const&) -> View = delete;
    auto operator=(View&&) -> View = delete;

    void registerMethod(std::string const& _methodName, auto func) {
        callablesFromClient.emplace(_methodName, [this, func](Json::Value _parameters) {
            using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
            auto paramsAsTuple = fon::json::deserialize<Params>(_parameters);
            std::apply([&](auto&&... params) {
                func(std::forward<decltype(params)>(params)...);
            }, paramsAsTuple);
        });
    }
    template <typename Self, typename ...Args>
    void registerMethod(this Self& self, std::string const& _methodName, void (Self::*func)(Args...)) {
        self.callablesFromClient.try_emplace(_methodName, [&self, func](Json::Value _parameters) {
            using Params = std::tuple<Args...>;
            auto paramsAsTuple = fon::json::deserialize<Params>(_parameters);
            std::apply([&](auto&&... params) {
                (self.*func)(std::forward<decltype(params)>(params)...);
            }, paramsAsTuple);
        });
    }
    void removeMethod(std::string const& _methodName) {
        callablesFromClient.erase(_methodName);
    }

    /**
     * Call function _methodName on all remote peers
     */
    template <typename ...Args>
    auto callAll(std::string_view _methodName, Args&&... _args) const {
        auto msg = detail::convertToMessage(_methodName, std::forward<Args>(_args)...);
        auto [guard, views] = *getViews();
        for (auto& _view : *views) {
            _view->sendData(msg);
        }
    }

    /**
     * Call function _methodName on the client associated with this View
     */
    template <typename ...Args>
    auto callBack(std::string_view _methodName, Args&&... _args) const {
        auto msg = detail::convertToMessage(_methodName, std::forward<Args>(_args)...);
        sendData(msg);
    }

    /**
     * Call function _methodName on all remote peers, but excluding the client
     * associated with this View
     */
    template <typename ...Args>
    auto callOthers(std::string_view _methodName, Args&&... _args) const {
        auto msg = detail::convertToMessage(_methodName, std::forward<Args>(_args)...);
        auto [guard, views] = *getViews();
        for (auto& _view : *views) {
            if (_view != this) {
                _view->sendData(msg);
            }
        }
    }
};

}
