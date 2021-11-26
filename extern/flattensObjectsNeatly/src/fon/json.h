#pragma once

#include "Visitor.h"
#include "proxy.h"
#include "traits.h"
#include "utils.h"

#include <cassert>
#include <json/json.h>

namespace fon::json {

namespace details {

struct Pointer {
    virtual ~Pointer() {}
};
template <typename T>
struct PointerT : Pointer {
    T const* ptr;
};

template <typename T>
auto serialize(T const& _input, Json::Value start = {}) -> Json::Value {
    auto& input = _input;

    Json::Value top;

    std::vector<std::unique_ptr<Pointer>> pointers;

    fon::visit([&]<typename Visitor, typename ValueT>(Visitor& visitor, ValueT const& obj) {

        auto stackVisit = [&](auto& obj) {
            Json::Value a = top;
            top.clear();
            visitor % obj;
            Json::Value b = top;
            top.swap(a);
            return b;
        };

        // Interpret int8_t and uint8_t as ints not chars
        if constexpr (std::is_same_v<ValueT, int8_t>
                    or std::is_same_v<ValueT, uint8_t>) {
            top = static_cast<int16_t>(obj);
        } else if constexpr (std::is_arithmetic_v<ValueT>
                      or std::is_same_v<std::string, ValueT>
                      or std::is_same_v<Json::Value, ValueT>) {
            top = obj;
        } else if constexpr (std::is_same_v<ValueT, std::string_view>
                            or std::is_same_v<ValueT, char const*>) {
            top = std::string{obj};
        } else if constexpr (fon::has_list_adapter_v<ValueT>) {
            auto adapter = fon::list_adapter{obj};
            adapter.visit([&](size_t key, auto& value) {
                auto right = stackVisit(value);
                top[top.size()] = right;
            });
        } else if constexpr (fon::has_map_adapter_v<ValueT>) {
            auto adapter = fon::map_adapter{obj};
            adapter.visit([&](auto& key, auto& value) {
                auto left  = stackVisit(key);
                auto right = stackVisit(value);
                top[left.asString()] = right;
            });
        } else if constexpr (fon::has_struct_adapter_v<ValueT>) {
            auto adapter = fon::struct_adapter{obj};
            adapter.visit([&](auto& key, auto& value) {
                auto left  = stackVisit(key);
                auto right = stackVisit(value);
                top[left.asString()] = right;
            }, [&](auto& value) {
                top = stackVisit(value);
            });
        } else if constexpr (std::is_pointer_v<ValueT>) {
            using UT = std::remove_pointer_t<ValueT>;
            fon::filter<UT>([&](auto& keys, auto& obj2) {
                if (&obj2 == obj) {
                    std::string key;
                    for (auto const& k : keys) {
                        key += "/" + k;
                    }
                    top = key;
                }
            }, _input);
        } else {
            []<bool flag = false>() {
                static_assert(fon::has_reflect_v<ValueT>, "fon: reflect or proxy missing (serialization)");
            }();
        }
    }, input);

    return top;
}


struct json_error : std::runtime_error {
    json_error(std::string s, Json::Value const& node)
        : runtime_error(s)
//        : runtime_error(s + " in line " + std::to_string(node.Mark().line) + ":" + std::to_string(node.Mark().column) + " (" + std::to_string(node.Mark().pos) + ")")
    {}
};

struct StackGuard final {
    std::vector<Json::Value>& stack;

    StackGuard(std::vector<Json::Value>& stack, Json::Value n)
        : stack{stack}
    {
        stack.push_back(n);
    }
    void reset(Json::Value n) {
        stack.pop_back();
        stack.push_back(n);
    }
    ~StackGuard() {
        stack.pop_back();
    }
};

template <typename T>
auto deserialize(Json::Value root) -> T {

    auto res = getEmpty<T>();
    std::vector<Json::Value> nodeStack{root};

    std::vector<std::function<void()>> fixPointers;
    fon::visit([&]<typename Visitor, typename ValueT>(Visitor& visitor, ValueT& obj) {
        auto top = nodeStack.back();

        if (top.isNull()) {
            return;
        }

        try {
            if constexpr (std::is_same_v<Json::Value, ValueT>) {
                obj = top;
            } else if constexpr (std::is_arithmetic_v<ValueT>
                      or std::is_same_v<std::string, ValueT>) {
                if constexpr (std::is_same_v<ValueT, int8_t>
                              or std::is_same_v<ValueT, uint8_t>
                              or std::is_same_v<ValueT, int16_t>
                              or std::is_same_v<ValueT, uint16_t>
                              or std::is_same_v<ValueT, int32_t>
                              or std::is_same_v<ValueT, uint32_t>) {
                    auto v = top.template as<int64_t>();
                    if (v < std::numeric_limits<ValueT>::min() or v > std::numeric_limits<ValueT>::max()) {
                        throw std::runtime_error("value out of range");
                    }
                    obj = v;
                } else {
                    // !TODO no check, we just hope it works
                    obj = top.template as<ValueT>();
                }
            } else if constexpr (fon::has_list_adapter_v<ValueT>) {
                auto adapter = fon::list_adapter<ValueT>{obj, top.size()};
                adapter.visit([&](size_t idx, auto& value) {
                    auto g = StackGuard{nodeStack, top[Json::ArrayIndex(idx)]};
                    visitor % value;
                });
            } else if constexpr (fon::has_map_adapter_v<ValueT>) {
                auto adapter = fon::map_adapter<ValueT>{obj, top.size()};
                auto members = top.getMemberNames();
                auto iter = begin(members);
                adapter.visit([&](auto& key, auto& value) {
                    auto g = StackGuard{nodeStack, *iter};
                    visitor % key;
                    g.reset(top[*iter]);
                    visitor % value;
                    ++iter;
                });
            } else if constexpr (fon::has_struct_adapter_v<ValueT>) {
                auto adapter = fon::struct_adapter{obj};
                adapter.visit([&](auto& key, auto& value) {
                    auto str_key = [&]() -> std::string {
                        using Key = std::decay_t<decltype(key)>;
                        if constexpr (std::is_same_v<Key, std::string>
                                    or std::is_same_v<Key, std::string_view>
                                    or std::is_same_v<Key, char const*>) {
                            return key;
                        } else if constexpr (std::is_arithmetic_v<Key>) {
                            return std::to_string(key);
                        } else {
                            []<bool flag = false>() {
                                static_assert(flag, "unknow type for key");
                            }();
                        }
                        return "";
                    }();
//                    auto str_key = Json::Value{key}.toStyledString();//Json::writeString(Json::StreamWriterBuilder{}, key);
                    auto g = StackGuard{nodeStack, top[str_key]};
                    visitor % value;
                }, [&](auto& value) {
                    visitor % value;
                });
            } else if constexpr (std::is_pointer_v<ValueT>) {
                std::string key = top.as<std::string>();
                fixPointers.push_back([&, key]() {
                    using UT = std::remove_pointer_t<ValueT>;
                    fon::filter<UT>([&]<typename Obj2>(auto& keys, Obj2 const& obj2) {
                        std::string key2;
                        for (auto const& k : keys) {
                            key2 += "/" + k;
                        }
                        if (key2 == key) {
                            obj = &const_cast<Obj2&>(obj2);
                        }
                    }, res);
                });
                // must be done later
            } else {
                []<bool flag = false>() {
                    static_assert(fon::has_reflect_v<ValueT>, "fon: reflect or proxy missing (deserialization)");
                }();
            }
        } catch(json_error const&) {
            throw;
        } catch(...) {
            std::throw_with_nested(json_error("error reading json file ", top));
        }
    }, res);

    // add missing pointers
    for (auto& f : fixPointers) {
        f();
    }

    return res;
}

}

using details::serialize;
using details::deserialize;

}
