#pragma once

#include "asFunction.h"

#include <any>
#include <fon/yaml.h>
#include <fon/std/all.h>
#include <map>
#include <string_view>
#include <unordered_set>

#include <fmt/format.h>

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

namespace detail2 {
template <typename ...Args>
auto to_yaml(Args&&...args) -> YAML::Node {
    return fon::yaml::serialize<std::tuple<std::decay_t<Args>...>>(std::tuple{std::forward<Args>(args)...});
}

template <typename ...Args>
auto convertToMessage(std::string_view _actionName, Args&&... _args) -> YAML::Node {
    auto node = YAML::Node{};
    node["action"]  = std::string{_actionName};
    node["params"]  = to_yaml(std::forward<Args>(_args)...);

    return node;
}
}


template <typename T>
struct View;

template <typename T>
struct Controller {
private:
    using ViewList   = std::unordered_set<T*>;

    ViewList   activeViews;
public:
    auto getViews() const -> auto const& {
        return activeViews;
    }

private:
    friend class View<T>;
    void dispatchSignalFromClient(View<T>& _view, YAML::Node msg) {
        auto& view = static_cast<T&>(_view);

        auto action = msg["action"].as<std::string>();

        // calls the correct function from ::reflect
        auto selector = detail::FunctionSelector{action, [&](auto func) {
            using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
            auto paramsAsTuple = fon::yaml::deserialize<Params>(msg["params"]);
            std::apply([&](auto&&... params) {
                (view.*func)(std::forward<decltype(params)>(params)...);
            }, paramsAsTuple);
        }};
        T::reflect(selector);
    }

    void removeView(View<T>& view) {
        auto& t = static_cast<T&>(view);
        activeViews.erase(&t);
    }
public:

    template <typename ...Args>
    auto makeView(std::function<void(YAML::Node)> _sendData, Args&&... args) -> std::unique_ptr<T> {
        View<T>::gSendData  = std::move(_sendData);
        View<T>::gController = this;
        auto view = std::make_unique<T>(std::forward<Args>(args)...);
        auto ptr = view.get();
        activeViews.insert(view.get());

        return view;
    }

    struct Call {
        Controller const& service;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const {
            auto msg = detail2::convertToMessage(actionName, std::forward<Args>(_args)...);
            for (auto& _view : service.getViews()) {
                _view->sendData(msg);
            }
        }
    };


    auto callAll(std::string_view _actionName) const {
        return Call{*this, _actionName};
    }
};

}
