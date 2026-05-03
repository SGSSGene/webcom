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

template <typename TModel>
struct Controller {
private:
    using ViewList = std::unordered_set<View*>;

    // List of all active views
    channel::value_mutex<ViewList> activeViews;

    // The actual underlying used model
    TModel model;
public:
    // Construction of the controller and
    // the underlying model
    template <typename ...Args>
    Controller(Args &&... _args)
        : model{std::forward<Args>(_args)...}
    {}

    // Create a view onto this object
    template <typename TView>
    auto makeView(std::function<void(Json::Value)> _sendData = [](Json::Value){}) -> std::unique_ptr<TView> {
        TView::gCTor = {
            .sendData    = std::move(_sendData),
            .activeViews = &activeViews,
        };

        auto view = std::make_unique<TView>(model);

        // register all callbacks
        {
            // register all the functions available in ::reflect
            auto cb = [&](std::string_view name, auto func) {
                view->registerMethod(std::string{name}, func);
            };
            if constexpr (requires() {
                TView::reflect(cb);
            }) {
                TView::reflect(cb);
            }
        }
        activeViews->insert(view.get());

        return view;
    }
};

}
