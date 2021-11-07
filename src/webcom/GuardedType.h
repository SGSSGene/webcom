#pragma once

#include <mutex>
#include <tuple>

namespace webcom {

/** Data Type thas must be locked to be accessible
 */
template <typename T>
class GuardedType {
    mutable std::mutex mutex;
    T          value;
public:
    auto operator*()&  {
        return std::tuple<std::lock_guard<std::mutex>, T&>{mutex, value};
    }
    auto operator*() const& {
        return std::tuple<std::lock_guard<std::mutex>, T const&>{mutex, value};
    }
};

}
