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


struct ViewContorller;

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
}

struct Service {
    using Dispatcher = std::function<void(std::string_view, std::any&, YAML::Node)>;

    std::string                                   name;
    std::function<std::any(ViewController&)>      objectCreate;
    Dispatcher                                    objectDispatch;
    std::unordered_map<ViewController*, std::any> remotes;


    template <typename CB>
    Service(std::string_view _name, CB cb)
        : name {_name}
    {
        using Return = typename webcom::signature_t<CB>::return_t;
        objectCreate = [cb](ViewController& viewController) -> std::any {
            return std::any(cb(viewController));
        };

        objectDispatch = [](std::string_view action, std::any& unknown_object, YAML::Node msg) {
            auto& object = std::any_cast<Return&>(unknown_object);

            auto selector = detail::FunctionSelector{action, [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto argsAsTuple = fon::yaml::deserialize<Params>(msg);
                std::apply([&](auto&&... args) {
                    ((*object).*func)(std::forward<decltype(args)>(args)...);
                }, argsAsTuple);
            }};
            using R2 = typename Return::element_type;
            R2::reflect(selector);
        };
    }

    /** does this Service have any connections?
     */
    operator bool() const {
        return not remotes.empty();
    }

    void addViewController(ViewController& viewController) {
        remotes.try_emplace(&viewController, objectCreate(viewController));
    }

    void removeViewController(ViewController& viewController) {
        remotes.erase(&viewController);
    }

    void dispatchSignalFromClient(std::string_view _name, ViewController& _viewController, YAML::Node _node) {
        auto iter = remotes.find(&_viewController);
        if (iter == end(remotes)) {
            return;
        }
        objectDispatch(_name, iter->second, _node);
    }
};

}
