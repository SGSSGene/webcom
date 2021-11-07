#pragma once

#include <functional>

namespace webcom {

template <typename R, typename ...Args, typename C>
auto signature_impl(R (C::*)(Args...)) -> std::function<R(Args...)>;

template <typename R, typename ...Args, typename C>
auto signature_impl(R (C::*)(Args...) const) -> std::function<R(Args...)>;


template <typename L, typename R=void, typename ...Args>
auto as_function_impl() {
    if constexpr (sizeof...(Args) == 0) {
        return &L::operator();
    } else {
        return &L::template operator()<Args...>;
    }
}
template <typename L, typename ...Args>
using asFunction = decltype(signature_impl<Args...>(as_function_impl<L, Args...>()));

template <typename ...Args>
struct signature;

template <typename R, typename ...Args>
struct signature<std::function<R(Args...)>> {
    using return_t = typename std::decay_t<R>;
    using params_t = typename std::tuple<std::decay_t<Args>...>;
};

template <typename L, typename ...Args>
struct signature<L, Args...> : signature<asFunction<L, Args...>> {
    using Base = signature<asFunction<L, Args...> >;
    using return_t = typename Base::return_t;
    using params_t = typename Base::params_t;
};

template <typename R, typename ...Args, typename C>
struct signature<R (C::*)(Args...) const> {
    using return_t = typename std::decay_t<R>;
    using params_t = typename std::tuple<std::decay_t<Args>...>;
};

template <typename R, typename ...Args, typename C>
struct signature<R (C::*)(Args...)> {
    using return_t = typename std::decay_t<R>;
    using params_t = typename std::tuple<std::decay_t<Args>...>;
};


template <typename L, typename ...Args>
using signature_t = signature<L, Args...>;

}
