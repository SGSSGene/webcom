#pragma once

#include "Service.h"

#include <fmt/format.h>

namespace webcom {

class User {
};

class Services {
private:
    std::map<std::string, Service> serviceList;
public:

    template <typename CB>
    void provideViewController(std::string const& _key, CB cb) {
        auto [iter, succ] = serviceList.try_emplace(_key, _key, cb);

        using R = typename signature<CB>::return_t;
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
