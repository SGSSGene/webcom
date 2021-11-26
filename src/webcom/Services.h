#pragma once

#include "Controller.h"
#include "GuardedType.h"
#include "View.h"

#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

class Services {
public:
//    GuardedType<std::unordered_map<std::string, Controller>> controllerList;
    using SendCB = std::function<void(YAML::Node)>;
    using CB     = std::function<std::unique_ptr<View<int>>(SendCB)>;

private:
    GuardedType<std::unordered_map<std::string, CB>> controllerList;
public:

/*    template <typename CB>
    auto makeController(std::string const& _key, CB cb) -> auto& {
        auto&& [guard, list] = *controllerList;

        auto [iter, succ] = list.try_emplace(_key, cb);
        return iter->second;
    }*/
    void addController(std::string_view _key, CB _cb) {
        auto&& [guard, list] = *controllerList;
        list.try_emplace(std::string{_key}, _cb);
    }

    auto subscribe(std::string_view _serviceName, SendCB _send) -> std::unique_ptr<View<int>> {
        auto&& [guard, list] = *controllerList;

        auto iter = list.find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == end(list)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second(std::move(_send));
    }
};

}
