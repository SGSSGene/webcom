// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "asFunction.h"
#include "utility.h"

#include <any>
#include <channel/value_mutex.h>
#include <fmt/format.h>
#include <fon/json.h>
#include <fon/std/all.h>
#include <map>
#include <string_view>
#include <unordered_set>

namespace webcom {

namespace detail {

template <typename CB>
struct FunctionSelector {
    std::string_view name;
    CB cb;

    template <typename F>
    void operator()(std::string_view action, F func) {
        if (name == action) {
            cb(func);
        }
    }
};

template <typename CB>
FunctionSelector(std::string_view, CB) -> FunctionSelector<CB>;

}

struct View;

template </*typename View, */typename TModel>
struct Controller {
private:
    using ViewList = std::unordered_set<View*>;

    channel::value_mutex<ViewList> activeViews;
public:
    auto getViews() const -> auto const& {
        return activeViews;
    }

    TModel model;

    template <typename ...Args>
    Controller(Args &&... _args)
        : model{std::forward<Args>(_args)...}
    {}

    template <typename TTT>
    auto makeView(std::function<void(Json::Value)> _sendData = [](Json::Value){}) -> std::unique_ptr<TTT> {
        TTT::gSendData  = std::move(_sendData);
        auto view = std::make_unique<TTT>(model);
        auto ptr = view.get();
        view->cleanup = [this, ptr]() {
            activeViews->erase(ptr);
        };
        view->getViews = [this]() -> channel::value_mutex<std::unordered_set<View*>> const& {
            return getViews();
        };
        view->dispatchSignalFromClient = [ptr](Json::Value msg) {
            auto& view = *ptr;

            auto action = msg["action"].as<std::string>();

            if (auto iter = view.callablesFromClient.find(action); iter != view.callablesFromClient.end()) {
                iter->second(msg["params"]);
            } else {
                // calls the correct function from ::reflect
                auto selector = detail::FunctionSelector{action, [&](auto func) {
                    using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                    auto paramsAsTuple = fon::json::deserialize<Params>(msg["params"]);
                    std::apply([&](auto&&... params) {
                        (view.*func)(std::forward<decltype(params)>(params)...);
                    }, paramsAsTuple);
                }};
                if constexpr (requires() {
                    TTT::reflect(selector);
                }) {
                    TTT::reflect(selector);
                }
            }
        };
        activeViews->insert(ptr);

        return view;
    }

};

}
