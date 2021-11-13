#pragma once

#include "Service.h"
#include "ViewController.h"

#include <fmt/format.h>
#include <unordered_map>

namespace webcom {

template <typename T>
class Services {
private:
    std::unordered_map<std::string, ServiceT<T>> serviceList;
public:

    template <typename CB>
    auto provideViewController(std::string const& _key, CB cb) -> auto& {
        auto [iter, succ] = serviceList.try_emplace(_key, cb);
        return iter->second;
    }

    template <typename CB>
    auto subscribe(std::string_view _serviceName, CB cb, T t) -> std::unique_ptr<ViewController> {
        return getService(_serviceName).createViewController(std::move(cb), std::move(t));
    }

    auto getService(std::string_view _key) -> ServiceT<T>& {
        auto iter = serviceList.find(std::string{_key}); //!TODO how to make it work with std::string_view
        if (iter == end(serviceList)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _key));
        }
        return iter->second;
    }
};

}
