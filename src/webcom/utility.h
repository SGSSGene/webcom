// SPDX-FileCopyrightText: 2025 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include <fon/json.h>
#include <string>
#include <tuple>

namespace webcom::detail {

// Converts the call '_methodName' with arguments '_args...' to a json object
template <typename ...Args>
auto convertToMessage(std::string_view _methodName, Args&&... _args) -> Json::Value {
    auto node = Json::Value{};
    node["action"] = std::string{_methodName};
    node["params"] = fon::json::serialize(
                        std::tuple{std::forward<Args>(_args)...}
                     );
    return node;
}

// Converts a json object into a call
auto callMethod(auto _func, Json::Value _parameters) {
    using Params = typename signature_t<std::decay_t<decltype(_func)>>::params_t;
    auto paramsAsTuple = fon::json::deserialize<Params>(_parameters);
    std::apply([&](auto&&... params) {
        _func(std::forward<decltype(params)>(params)...);
    }, paramsAsTuple);
}

}
