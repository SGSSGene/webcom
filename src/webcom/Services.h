#pragma once

#include "Service.h"

#include <fmt/format.h>

namespace webcom {

class Services {
private:
    std::map<std::string, Service> serviceList;
public:

    template <typename CB>
    auto provideViewController(std::string const& _key, CB cb) -> auto& {
        auto [iter, succ] = serviceList.try_emplace(_key, _key, cb);
        return iter->second;
    }

    template <typename CB>
    auto subscribe(std::string_view _serviceName, CB cb) -> std::unique_ptr<ViewController> {
        return getService(_serviceName).createViewController(std::move(cb));
    }

    auto getService(std::string_view _key) -> Service& {
        auto iter = serviceList.find(std::string{_key}); //!TODO how to make it work with std::string_view
        if (iter == end(serviceList)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _key));
        }
        return iter->second;
    }
};

}
