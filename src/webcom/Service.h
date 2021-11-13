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

struct ViewController;

struct Service {
protected:
    using Dispatcher     = std::function<void(ViewController&, YAML::Node)>;
    using ControllerList = std::unordered_set<ViewController*>;

    Dispatcher     viewControllerDispatcher;
    ControllerList activeViewControllers;

public:
    auto getViewControllers() const -> auto const& {
        return activeViewControllers;
    }

    void removeViewController(ViewController& viewController) {
        activeViewControllers.erase(&viewController);
    }

    void dispatchSignalFromClient(ViewController& _viewController, YAML::Node _node) {
        viewControllerDispatcher(_viewController, _node);
    }
};

template <typename T>
struct ServiceT : Service {
private:
    using Factory = std::function<std::unique_ptr<ViewController>(T)>;

    Factory viewControllerFactory;

public:
    template <typename CB>
    ServiceT(CB cb) {
        viewControllerFactory = std::move(cb);

        viewControllerDispatcher = [](ViewController& viewController, YAML::Node msg) {
            using TypedViewController = typename webcom::signature_t<CB>::return_t::element_type;
            auto& typedViewController = dynamic_cast<TypedViewController&>(viewController);

            auto selector = detail::FunctionSelector{msg["action"].as<std::string>(), [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto paramsAsTuple = fon::yaml::deserialize<Params>(msg["params"]);
                std::apply([&](auto&&... params) {
                    (typedViewController.*func)(std::forward<decltype(params)>(params)...);
                }, paramsAsTuple);
            }};
            TypedViewController::reflect(selector);
        };
    }

    template <typename X = ViewController>
    auto createViewController(std::function<void(YAML::Node)> _sendData, T _userData) -> std::unique_ptr<ViewController> {
        X::gSendData = std::move(_sendData);
        X::gService = this;
        auto viewController = viewControllerFactory(std::move(_userData));
        auto ptr = viewController.get();
        activeViewControllers.insert(viewController.get());

        return viewController;
    }

    struct Call {
        ServiceT const& service;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const;
    };


    auto callAll(std::string_view _actionName) const {
        return Call{*this, _actionName};
    }
};

}

#include "ViewController.h"

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

template <typename T>
template <typename ...Args>
void ServiceT<T>::Call::operator()(Args&&... _args) const {
    auto msg = detail2::convertToMessage(actionName, std::forward<Args>(_args)...);
    for (auto& _viewController : service.getViewControllers()) {
        _viewController->sendData(msg);
    }
}

}
