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

struct View;

struct Controller {
private:
    using Factory    = std::function<std::unique_ptr<View>()>;
    using ViewList   = std::unordered_set<View*>;
    using Dispatcher = std::function<void(View&, YAML::Node)>;


    Factory    viewFactory;
    ViewList   activeViews;
    Dispatcher viewDispatcher;

public:
    template <typename CB>
    Controller(CB cb) {
        viewFactory = std::move(cb);

        viewDispatcher = [](View& view, YAML::Node msg) {
            using TypedView = typename webcom::signature_t<CB>::return_t::element_type;
            auto& typedView = dynamic_cast<TypedView&>(view);

            auto selector = detail::FunctionSelector{msg["action"].as<std::string>(), [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto paramsAsTuple = fon::yaml::deserialize<Params>(msg["params"]);
                std::apply([&](auto&&... params) {
                    (typedView.*func)(std::forward<decltype(params)>(params)...);
                }, paramsAsTuple);
            }};
            TypedView::reflect(selector);
        };
    }

    auto getViews() const -> auto const& {
        return activeViews;
    }

    void removeView(View& view) {
        activeViews.erase(&view);
    }

    void dispatchSignalFromClient(View& _view, YAML::Node _node) {
        viewDispatcher(_view, _node);
    }

    auto createView(std::function<void(YAML::Node)> _sendData) -> std::unique_ptr<View>;

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

inline auto Controller::createView(std::function<void(YAML::Node)> _sendData) -> std::unique_ptr<View> {
    View::gSendData   = std::move(_sendData);
    View::gController = this;
    auto view = viewFactory();
    auto ptr = view.get();
    activeViews.insert(view.get());

    return view;
}



}