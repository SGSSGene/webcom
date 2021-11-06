#pragma once

#include "asFunction.h"

#include <any>
#include <flattensObjectsNeatly/flattensObjectsNeatly.h>
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
auto convertToMessage(std::string_view _serviceName, std::string_view _actionName, Args&&... _args) -> YAML::Node {
    auto node = YAML::Node{};
    node["service"] = std::string{_serviceName};
    node["action"]  = std::string{_actionName};
    node["params"]  = to_yaml(std::forward<Args>(_args)...);
    return node;
}


struct Adapter;

struct Service {
    using Dispatcher = std::function<void(std::string_view, std::any&, YAML::Node)>;

    std::string                             name;
    std::function<std::any(Adapter&)>       objectCreate;
    Dispatcher                              objectDispatch;
    std::unordered_map<Adapter*, std::any>  objects;


    template <typename CB>
    Service(std::string_view _name, CB cb)
        : name {_name}
    {
        using Return = typename webcom::signature_t<CB>::return_t;
        objectCreate = [cb](Adapter& adapter) -> std::any {
            return std::any(cb(adapter));
        };

        objectDispatch = [](std::string_view action, std::any& unknown_object, YAML::Node msg) {
            auto& object = std::any_cast<Return&>(unknown_object);

            fon::visit([&](auto& node, auto& obj) {
                using Node   = std::decay_t<decltype(node)>;
                Node::range(obj, [&](auto& key, auto& value) {
                    if (key == action) {
                        using Params = typename signature_t<std::decay_t<decltype(value)>>::params_t;

                        auto argsAsTuple = fon::yaml::deserialize<Params>(msg);
                        std::apply([&](auto&&... args) {
                            ((*object).*value)(std::forward<decltype(args)>(args)...);
                        }, argsAsTuple);
                    }
                }, [&](auto& value) {
                    node % value;
                });
            }, *object);
        };
    }

    operator bool() const {
        return not objects.empty();
    }

    template <typename ...Args>
    void sendAll(std::string_view _actionName, Args&&... _args);

    void addAdapter(Adapter& adapter) {
        objects.try_emplace(&adapter, objectCreate(adapter));
    }

    void removeAdapter(Adapter& adapter) {
        objects.erase(&adapter);
    }

    void dispatchSignalFromClient(std::string_view _name, Adapter& _adapter, YAML::Node _node) {
        auto iter = objects.find(&_adapter);
        if (iter == end(objects)) {
            return;
        }
        objectDispatch(_name, iter->second, _node);
    }
};
}
#include "Adapter.h"
namespace webcom {
    template <typename ...Args>
    void Service::sendAll(std::string_view _actionName, Args&&... _args) {
        auto msg = convertToMessage(name, _actionName, std::forward<Args>(_args)...);
        for (auto& [adapter, value] : objects) {
            adapter->sendData(msg);
        }
    }
}
