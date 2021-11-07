#pragma once

#include "Service.h"

#include <fmt/format.h>

namespace webcom {

struct Services {
    std::map<std::string, Service> serviceList;

    template <typename CB>
    auto provideViewController(std::string const& _key, CB cb) {
        auto [iter, succ] = serviceList.try_emplace(_key, _key, cb);

        using R = typename signature<CB>::return_t;
        return TypedService<typename R::element_type>{iter->second};
    }

    auto& getService(std::string const& _key) {
        auto iter = serviceList.find(_key);
        if (iter == end(serviceList)) {
            throw std::runtime_error(fmt::format("unknown service \"{}\"", _key));
        }
        return iter->second;
    }
};

}
