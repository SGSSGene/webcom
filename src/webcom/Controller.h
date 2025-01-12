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


template <typename T>
struct View;

template <typename T>
struct Controller {
private:
    using ViewList = std::unordered_set<T*>;

    channel::value_mutex<ViewList> activeViews;
public:
    auto getViews() const -> auto const& {
        return activeViews;
    }

private:
    friend class View<T>;
    void removeView(View<T>& view) {
        auto& t = static_cast<T&>(view);
        activeViews->erase(&t);
    }
public:

    template <typename ...Args>
    auto makeView(std::function<void(Json::Value)> _sendData, Args&&... args) -> std::unique_ptr<T> {
        View<T>::gSendData  = std::move(_sendData);
        View<T>::gController = this;
        auto view = std::make_unique<T>(std::forward<Args>(args)...);
        auto ptr = view.get();
        activeViews->insert(view.get());

        return view;
    }
    template <typename ...Args>
    auto makeView2(Args&&... args) -> std::unique_ptr<T> {
        auto sendFunc = [](Json::Value) {};
        return makeView(sendFunc, std::forward<Args>(args)...);
    }
};

}
