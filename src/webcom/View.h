// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "utility.h"

#include <functional>
#include <channel/channel.h>
#include <fon/json.h>
#include <fon/std/all.h>

#ifdef WEBCOM_USE_REFLECTION
#include <meta>
#include <print>
#endif

namespace webcom {

struct View {
    using SendData = std::function<void(Json::Value)>;
    using ViewList = std::unordered_set<View*>;

    // By passing constructor function for easier implementation
    struct CTor {
        SendData sendData;
        channel::value_mutex<ViewList>* activeViews;

    };
    thread_local static inline CTor gCTor;

    // function to send data to client
    SendData sendData{std::move(gCTor.sendData)};

    // list of views to controller/model
    channel::value_mutex<ViewList>& activeViews{*gCTor.activeViews};

    using Callable = std::function<void(Json::Value)>;
    std::unordered_map<std::string, Callable> callablesFromClient;

    View() = default;
    View(View const&) = delete;
    View(View&&) = delete;

    virtual ~View() {
        activeViews->erase(this);
    }

    auto operator=(View const&) -> View = delete;
    auto operator=(View&&) -> View = delete;

    void init() const {
        auto names = std::vector<std::string_view>{};
        for (auto const& [key, cb] : callablesFromClient) {
            names.push_back(key);
        }
        callBack("__ctor", names);
    }

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
    #ifdef WEBCOM_USE_REFLECTION
    template <typename T>
    void registerAllMethods(this T&& self) {
        using S = std::decay_t<T>;
        template for (constexpr auto x : define_static_array(std::meta::members_of(^^S, std::meta::access_context::current()))) {
            if constexpr (is_nonstatic_data_member(x)) {
            } else if constexpr (is_function(x) && !is_defaulted(x) && !is_constructor(x)) {
                self.registerMethod(std::format("{}", identifier_of(x)), &[: x :]);
            }
        }
    }
    #endif
    void removeMethod(std::string const& _methodName) {
        callablesFromClient.erase(_methodName);
    }

    struct BulkCall {
        View const& view;
        Json::Value node{};
        template <typename ...Args>
        auto add(std::string_view _methodName, Args&&... _args) -> BulkCall& {
            node.append(detail::convertToMessage(_methodName, std::forward<Args>(_args)...));
            return *this;
        }
        void sendAll() {
            auto [guard, views] = *view.activeViews;
            for (auto& _view : *views) {
                _view->sendData(node);
            }
        }
        void sendBack() {
            view.sendData(node);
        }
        void sendOthers() {
            auto [guard, views] = *view.activeViews;
            for (auto& _view : *views) {
                if (_view != &view) {
                    _view->sendData(node);
                }
            }
        }
    };
    auto bulkCall() const -> BulkCall {
        return {*this};
    };

    /**
     * Call function _methodName on all remote peers
     */
    template <typename ...Args>
    void callAll(std::string_view _methodName, Args&&... _args) const {
        bulkCall().add(_methodName, std::forward<Args>(_args)...).sendAll();
    }

    /**
     * Call function _methodName on the client associated with this View
     */
    template <typename ...Args>
    void callBack(std::string_view _methodName, Args&&... _args) const {
        bulkCall().add(_methodName, std::forward<Args>(_args)...).sendBack();
    }

    /**
     * Call function _methodName on all remote peers, but excluding the client
     * associated with this View
     */
    template <typename ...Args>
    void callOthers(std::string_view _methodName, Args&&... _args) const {
        bulkCall().add(_methodName, std::forward<Args>(_args)...).sendOthers();
    }

    /**
     * Dispatches the signal given by msg to the correct method from the view
     */
    void dispatchSignalFromClient(Json::Value msg) {
        auto action = msg["action"].as<std::string>();
        if (auto iter = callablesFromClient.find(action); iter != callablesFromClient.end()) {
            iter->second(msg["params"]);
        } else {
            throw std::runtime_error{"client called invalid function"};
        }
    }
};

}
