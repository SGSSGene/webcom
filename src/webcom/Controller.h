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

template <typename T>
struct View;

struct Controller {
private:
    using ViewList   = std::unordered_set<View<int>*>;
    using Dispatcher = std::function<void(View<int>&, YAML::Node)>;

    ViewList   activeViews;
    Dispatcher viewDispatcher;

public:
    auto getViews() const -> auto const& {
        return activeViews;
    }

private:
    friend class View<int>;
    void dispatchSignalFromClient(View<int>& _view, YAML::Node _node) {
        viewDispatcher(_view, _node);
    }

    void removeView(View<int>& view) {
        activeViews.erase(&view);
    }
public:

    template <typename TView, typename ...Args>
    auto makeView(std::function<void(YAML::Node)> _sendData, Args&&... args) -> std::unique_ptr<View<int>>;

    struct Call {
        Controller const& service;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const;
    };


    auto callAll(std::string_view _actionName) const {
        return Call{*this, _actionName};
    }
};

}

#include "View.h"

namespace webcom {
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

template <typename ...Args>
inline void Controller::Call::operator()(Args&&... _args) const {
    auto msg = detail2::convertToMessage(actionName, std::forward<Args>(_args)...);
    for (auto& _view : service.getViews()) {
        _view->sendData(msg);
    }
}

/*inline auto Controller::createView(std::function<void(YAML::Node)> _sendData) -> std::unique_ptr<View<int>> {
    View<int>::gSendData   = std::move(_sendData);
    View<int>::gController = this;
    auto view = viewFactory();
    auto ptr = view.get();
    activeViews.insert(view.get());

    return view;
}*/
template <typename TView, typename ...Args>
inline auto Controller::makeView(std::function<void(YAML::Node)> _sendData, Args&&... args) -> std::unique_ptr<View<int>> {
    if (!viewDispatcher) {
        viewDispatcher = [](View<int>& view, YAML::Node msg) {
            auto& typedView = dynamic_cast<TView&>(view);

            auto action = msg["action"].as<std::string>();

            auto selector = detail::FunctionSelector{action, [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto paramsAsTuple = fon::yaml::deserialize<Params>(msg["params"]);
                std::apply([&](auto&&... params) {
                    (typedView.*func)(std::forward<decltype(params)>(params)...);
                }, paramsAsTuple);
            }};
            TView::reflect(selector);
        };
    }

    View<int>::gSendData   = std::move(_sendData);
    View<int>::gController = this;
    auto view = std::make_unique<TView>(std::forward<Args>(args)...);
    auto ptr = view.get();
    activeViews.insert(view.get());

    return view;
}


}
