#pragma once

#include "Controller.h"
#include "GuardedType.h"
#include "View.h"

#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

template <typename T>
class Services {
private:
    GuardedType<std::unordered_map<std::string, ControllerT<T>>> controllerList;
public:

    template <typename CB>
    auto provideView(std::string const& _key, CB cb) -> auto& {
        auto&& [guard, list] = *controllerList;

        auto [iter, succ] = list.try_emplace(_key, cb);
        return iter->second;
    }

    template <typename CB>
    auto subscribe(std::string_view _serviceName, CB cb, T t) -> std::unique_ptr<View> {
        auto&& [guard, list] = *controllerList;

        auto iter = list.find(std::string{_serviceName}); //!TODO how to make it work with std::string_view
        if (iter == end(list)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _serviceName));
        }
        return iter->second.createView(std::move(cb), std::move(t));
    }

    auto getController(std::string_view _key) -> ControllerT<T>& {
        auto&& [guard, list] = *controllerList;
        auto iter = list.find(std::string{_key}); //!TODO how to make it work with std::string_view
        if (iter == end(list)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _key));
        }
        return iter->second;
    }
};

}
