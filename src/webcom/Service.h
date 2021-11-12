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

struct Service {
    using Dispatcher = std::function<void(ViewController&, YAML::Node)>;

    template <typename> friend struct FunctionSelector;
private:
    std::function<std::unique_ptr<ViewController>()>  objectCreate;
    Dispatcher                                        objectDispatch;
    std::unordered_set<ViewController*>               viewControllers;
public:

    template <typename CB>
    Service(CB cb) {
        objectCreate = std::move(cb);

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
