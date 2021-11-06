#pragma once

#include <filesystem>

#include "proxy.h"

namespace fon {

template <>
struct proxy<std::filesystem::path> {
    static constexpr void reflect(auto& visitor, auto& self) {
        std::string str;
        visitor % str;
        self = str;
    }

    static constexpr void reflect(auto& visitor, auto const& self) {
        std::string str = self;
        visitor % str;
    }
};

}
