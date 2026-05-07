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

struct View;

template <typename TModel, typename TView = std::remove_cvref_t<TModel>::View>
struct Controller {
private:
    using ViewList = std::unordered_set<View*>;

    // List of all active views
    channel::value_mutex<ViewList> activeViews;

    // The actual underlying used model
    TModel model;

    // a view that does not send anything out
    std::unique_ptr<TView> view = makeView([](Json::Value){});


public:
    // Construction of the controller and
    // the underlying model
    template <typename ...Args>
    Controller(Args &&... _args)
        : model{std::forward<Args>(_args)...}
    {}

    // Create a view onto this object
    auto makeView(std::function<void(Json::Value)> _sendData) -> std::unique_ptr<TView> {
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
        view->init();
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
