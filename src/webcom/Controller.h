// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "View.h"

#include <channel/value_mutex.h>
#include <fmt/format.h>
#include <fon/json.h>
#include <fon/std/all.h>
#include <map>
#include <string_view>
#include <unordered_set>

namespace webcom {

struct View;

struct ControllerBase {
    virtual auto makeView(std::function<void(Json::Value)>) -> std::unique_ptr<View> = 0;
    virtual ~ControllerBase() = default;
};

template <typename TModel, typename TView = std::remove_cvref_t<TModel>::View>
struct Controller : ControllerBase {
private:
    using ViewList = std::unordered_set<View*>;

    // List of all active views
    channel::value_mutex<ViewList> activeViews;

    // The actual underlying used model
    TModel model;

    // a view that does not send anything out
    std::unique_ptr<TView> view = makeViewT([](Json::Value){});


public:
    // Construction of the controller and
    // the underlying model
    template <typename ...Args>
    Controller(Args &&... _args)
        : model{std::forward<Args>(_args)...}
    {}

    auto makeView(std::function<void(Json::Value)> _sendData) -> std::unique_ptr<View> override {
        return makeViewT(std::move(_sendData));
    }

    // Create a view onto this object
    auto makeViewT(std::function<void(Json::Value)> _sendData) -> std::unique_ptr<TView> {
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
        static_cast<View*>(view.get())->init();
        activeViews->insert(view.get());

        return view;
    }

    auto operator*(this auto&& self) -> auto& {
        return *self.view.get();
    }
    auto operator->(this auto&& self) {
        return self.view.get();
    }
};

}
