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


struct ViewBase;

template <typename T>
struct View;

template </*typename View, */typename TModel>
struct Controller {
private:
    using ViewList = std::unordered_set<ViewBase*>;

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

private:
    friend class ViewBase;
    void removeView(ViewBase& view) {
        activeViews->erase(&view);
    }
public:

    template <typename TTT, typename ...Args>
    auto makeView(std::function<void(Json::Value)> _sendData, Args&&... args) -> std::unique_ptr<TTT> {
        View<TTT>::gSendData  = std::move(_sendData);
        auto view = std::make_unique<TTT>(std::forward<Args>(args)...);
        auto ptr = view.get();
        view->cleanup = [this, ptr]() {
            removeView(*ptr);
        };
        view->getViews = [this]() -> channel::value_mutex<std::unordered_set<ViewBase*>> const& {
            return getViews();
        };
        activeViews->insert(ptr);

        return view;
    }
    template <typename TTT, typename ...Args>
    auto makeView2(Args&&... args) -> std::unique_ptr<TTT> {
        auto sendFunc = [](Json::Value) {};
        return makeView<TTT>(sendFunc, std::forward<Args>(args)...);
    }
};

}
