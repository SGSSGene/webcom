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
auto convertToMessage(std::string_view _serviceName, std::string_view _actionName, Args&&... _args) -> std::string {
    auto node = YAML::Node{};
    node["service"] = std::string{_serviceName};
    node["action"]  = std::string{_actionName};
    node["params"]  = to_yaml(std::forward<Args>(_args)...);

    YAML::Emitter emit;
    emit << node;
    std::stringstream ss;
    ss << emit.c_str();
    return ss.str();
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

}

struct Service {
    using Dispatcher = std::function<void(std::string_view, ViewControllerBase&, YAML::Node)>;

    template <typename> friend struct FunctionSelector;
private:
    std::string                                                              name;
    std::function<std::unique_ptr<ViewControllerBase>(ViewController&)>      objectCreate;
    Dispatcher                                                               objectDispatch;
    std::unordered_map<ViewController*, std::unique_ptr<ViewControllerBase>> viewControllers;
public:

    template <typename CB>
    Service(std::string_view _name, CB cb)
        : name {_name}
    {
        using Return = typename webcom::signature_t<CB>::return_t::element_type;
        objectCreate = [cb](ViewController& viewController) -> std::unique_ptr<ViewControllerBase> {
            return cb(viewController);
        };

        objectDispatch = [](std::string_view action, ViewControllerBase& unknown_object, YAML::Node msg) {
            auto& object = dynamic_cast<Return&>(unknown_object);

            auto selector = detail::FunctionSelector{action, [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto argsAsTuple = fon::yaml::deserialize<Params>(msg);
                std::apply([&](auto&&... args) {
                    (object.*func)(std::forward<decltype(args)>(args)...);
                }, argsAsTuple);
            }};
            Return::reflect(selector);
        };
    }

    auto getName() const -> std::string_view {
        return name;
    }
    auto getViewControllers() const -> std::unordered_map<ViewController*, std::unique_ptr<ViewControllerBase>> const& {
        return viewControllers;
    }


    /** does this Service have any connections?
     */
    operator bool() const {
        return not viewControllers.empty();
    }

    auto createViewController(std::function<void(std::string_view)>& _sendData, std::function<size_t()>& _getSize) -> std::unique_ptr<ViewController> {
        auto viewController = std::make_unique<ViewController>(_sendData, _getSize, *this);
        addViewController(*viewController);
        return viewController;
    }



    void addViewController(ViewController& viewController) {
        viewControllers.try_emplace(&viewController, objectCreate(viewController));
    }

    void removeViewController(ViewController& viewController) {
        viewControllers.erase(&viewController);
    }

    void dispatchSignalFromClient(std::string_view _name, ViewController& _viewController, YAML::Node _node) {
        auto iter = viewControllers.find(&_viewController);
        if (iter == end(viewControllers)) {
            return;
        }
        objectDispatch(_name, *iter->second, _node);
    }
};

}
