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

template <typename ...Args>
auto to_yaml(Args&&...args) -> YAML::Node {
    return fon::yaml::serialize<std::tuple<std::decay_t<Args>...>>(std::tuple{std::forward<Args>(args)...});
}

template <typename ...Args>
auto convertToMessage(std::string_view _serviceName, std::string_view _actionName, Args&&... _args) -> YAML::Node {
    auto node = YAML::Node{};
    node["action"]  = std::string{_actionName};
    node["params"]  = to_yaml(std::forward<Args>(_args)...);

    return node;
}

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

struct Service {
    using Dispatcher = std::function<void(ViewController&, YAML::Node)>;

    template <typename> friend struct FunctionSelector;
private:
    std::string                                       name;
    std::function<std::unique_ptr<ViewController>()>  objectCreate;
    Dispatcher                                        objectDispatch;
    std::unordered_set<ViewController*>               viewControllers;
public:

    template <typename CB>
    Service(std::string _name, CB cb)
        : name {std::move(_name)}
    {
        objectCreate = [cb]() -> std::unique_ptr<ViewController> {
            return cb();
        };

        objectDispatch = [](ViewController& viewController, YAML::Node msg) {
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

    auto getName() const -> std::string_view {
        return name;
    }
    auto getViewControllers() const -> std::unordered_set<ViewController*> const& {
        return viewControllers;
    }


    auto createViewController(std::function<void(YAML::Node)> _sendData) -> std::unique_ptr<ViewController> {
        ViewController::gSendData = std::move(_sendData);
        ViewController::gService = this;
        auto viewController = objectCreate();
        auto ptr = viewController.get();
        viewControllers.insert(viewController.get());

        return viewController;
    }

    void removeViewController(ViewController& viewController) {
        viewControllers.erase(&viewController);
    }

    void dispatchSignalFromClient(ViewController& _viewController, YAML::Node _node) {
        objectDispatch(_viewController, _node);
    }
};

}
