#include <fon/Visitor.h>
#include <fon/yaml.h>
#include <fon/std/all.h>

#include <bitset>
#include <chrono>
#include <cstring>
#include <functional>
#include <iostream>
#include <tuple>
#include <vector>

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

struct WeirdType {
    int x;
};

struct DepthType {
    int x;

    static constexpr void reflect(auto& visitor, auto& self) {
        visitor["x"] % self.x;
    }
};

enum class MyEnum {
    A,
    B,
    C,
};

struct A {
    int x;
    int y;
    MyEnum myEnum{MyEnum::A};
    WeirdType z;
    DepthType w;

    std::vector<int> l;

    template <typename Self>
    static constexpr void reflect(auto& visitor, Self& self) {
        visitor["x"] % self.x;
        visitor["y"] % self.y;
        visitor["z"] % self.z.x;
        visitor["w"] % self.w;
        visitor["enum"] % self.myEnum;
        std::cout << typeid(Self).name() << "\n";
        if constexpr (std::is_const_v<Self>) {
            std::cout << "is const\n";
        } else {
            std::cout << "is not const\n";
        }
    }
};

void serialize(auto const& a) {
    fon::visit(overloaded{[](int const& x, auto const&) {
        std::cout << "accessing1: " << typeid(x).name() << "\n";
    }, []<typename T>(T const& x, auto const& visitor) {
        std::cout << "accessing2: " << typeid(x).name() << "\n";
        visitor(x);
    }}, a);
}
void deserialize(auto& a) {
    fon::visit(overloaded{[](int& x, auto const&) {
        std::cout << "accessing1: " << typeid(x).name() << "\n";
    }, []<typename T>(T& x, auto const& visitor) {
        std::cout << "accessing2: " << typeid(x).name() << "\n";
        visitor(x);
    }}, a);
}

struct B {
    int x{5};
    static constexpr void reflect(auto& visitor, auto& self) {
        visitor["x"] % self.x;
    }
};

struct C {
    int a = 1;
    std::vector<int> b{1, 2, 3};
    std::map<int, int> c{{101, 1}, {102, 2}, {103, 3}};
    B d;
    static constexpr void reflect(auto& visitor, auto& self) {
        visitor["a"] % self.a;
        visitor["b"] % self.b;
        visitor["c"] % self.c;
        visitor["d"] % self.d;
    }
};

int main() {
//    int a = 1;
//    std::vector<int> a{1, 2, 3};
//    auto a = std::map<int, int>{{101, 1}, {102, 2}, {103, 3}};
    C a;
    a.a += 1000;
    for (auto& e : a.b) {
        e += 1000;
    }
    for (auto& [key, value] : a.c) {
        value += 1000;
    }
    a.d.x += 1000;

    static_assert(fon::has_struct_adapter_v<B>);

//    static_assert(fon::has_list_adapter_v<std::set<int>>);
//    A a;
//    deserialize(a);
//    serialize(a);

    auto node = fon::yaml::serialize(a);

    std::cout << "node: " << node << "\n";

    auto a2 = fon::yaml::deserialize<decltype(a)>(node);
    {
        auto node2 = fon::yaml::serialize(a2);
        std::cout << "node2: " << node2 << "\n";
    }


}
