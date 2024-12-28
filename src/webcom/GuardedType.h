// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <mutex>
#include <tuple>

namespace webcom {

template <typename T>
class LockGuard {
    std::lock_guard<std::mutex> lock;
    T&                          value;
public:
    LockGuard(std::mutex& _mutex, T& _value)
        : lock{_mutex}
        , value{_value}
    {}
    auto operator->() && -> T* {
        return &value;
    }
    auto operator->() const&& -> T const* {
        return &value;
    }

};

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
    auto operator->() -> LockGuard<T> {
        return LockGuard{mutex, value};
    }
    auto operator->() const -> LockGuard<T const> {
        return LockGuard{mutex, value};
    }
};

}
