#pragma once

#include "Service.h"

#include <fmt/format.h>

namespace webcom {
namespace detail {
struct EmptyData {
    template <typename Node>
    void serialize(Node& node) {}
};
}

struct Services {
    std::map<std::string, Service> serviceList;

    template <typename CB>
    auto emplace(std::string const& _key, CB cb) {
        auto [iter, succ] = serviceList.try_emplace(_key, _key, cb);

        using R = typename signature<CB>::return_t;
        return TypedService<typename R::element_type>{iter->second};
    }

    auto emplace(std::string const& _key) -> TypedService<detail::EmptyData> {
        auto [iter, succ] = serviceList.try_emplace(_key, _key, [](Adapter& adapter) {
            return adapter.make<detail::EmptyData>();
        });
        return {iter->second};
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
