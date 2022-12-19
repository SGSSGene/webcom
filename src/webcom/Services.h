#pragma once

#include "Controller.h"
#include "GuardedType.h"
#include "View.h"

#include <cassert>
#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

class Services {
public:
    using SendCB = std::function<void(Json::Value)>;
    using CB     = std::function<std::unique_ptr<ViewBase>(SendCB)>;

private:
    GuardedType<std::unordered_map<std::string, CB>> controllerList;

public:
    void addController(std::string_view _key, CB _cb) {
        assert(_cb);
        auto&& [guard, list] = *controllerList;
        list.try_emplace(std::string{_key}, _cb);
    }

    auto subscribe(std::string_view _serviceName, SendCB _send) -> std::unique_ptr<ViewBase> {
        auto&& [guard, list] = *controllerList;

        auto iter = list.find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == end(list)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second(std::move(_send));
    }
};

}
