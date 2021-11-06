#include <catch2/catch.hpp>
#include <fon/yaml.h>
#include <fon/std/all.h>

namespace {


struct A {
    std::vector<int32_t> xs;
    int32_t* ptr{nullptr};

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["xs"]  % self.xs;
        visitor["ptr"] % self.ptr;
    }
};

TEST_CASE("test yaml serialization of raw pointers", "[yaml][raw][pointer][serialize]") {
    auto data = A{{10, 20, 30}};
    data.ptr = &data.xs[1];

    auto node = fon::yaml::serialize(data);

    REQUIRE(node.IsMap());
    REQUIRE(node["ptr"].as<std::string>() == "/xs/1");
}

TEST_CASE("test yaml deserialization of raw pointers", "[yaml][raw][pointer][deserialize]") {
    YAML::Node node;
    node["xs"][0] = 10;
    node["xs"][1] = 20;
    node["xs"][2] = 30;
    node["ptr"] = "/xs/1";

    auto data = fon::yaml::deserialize<A>(node);

    REQUIRE(data.xs == (std::vector<int32_t>{10, 20, 30}));
    REQUIRE(data.ptr == &data.xs[1]);
}

TEST_CASE("test yaml serialization of unique_ptr pointers", "[yaml][std][unique_ptr][pointer][serialize]") {
    auto data = std::make_unique<int32_t>(42);
    auto node = fon::yaml::serialize(data);
    REQUIRE(node.IsSequence());
    CHECK(node[0].as<int32_t>() == 42);
}

TEST_CASE("test yaml deserialization of unique_ptr pointers", "[yaml][std][unique_ptr][pointer][deserialize]") {
    YAML::Node node;
    node[0] = 42;

    auto data = fon::yaml::deserialize<std::unique_ptr<int32_t>>(node);
    REQUIRE(data);
    CHECK(*data == 42);
}

struct B {
    std::unique_ptr<int32_t> uptr;
    int* ptr{nullptr};

    static void reflect(auto& visitor,auto& self) {
        visitor["uptr"] % self.uptr;
        visitor["ptr"]  % self.ptr;
    }
};

TEST_CASE("test yaml serialization of unique_ptr pointers and raw pointers", "[yaml][std][unique_ptr][raw][pointer][serialize][struct]") {
    auto data = B{};
    data.uptr = std::make_unique<int32_t>(42);
    data.ptr = data.uptr.get();

    auto node = fon::yaml::serialize(data);

    REQUIRE(node.IsMap());
    REQUIRE(node["uptr"].IsSequence());
    CHECK(node["uptr"][0].as<int32_t>() == 42);
    CHECK(node["ptr"].as<std::string>() == "/uptr/0");
}

TEST_CASE("test yaml deserialization of unique_ptr pointers and raw pointers", "[yaml][std][unique_ptr][raw][pointer][deserialize][struct]") {
    YAML::Node node;
    node["uptr"][0] = 43;
    node["ptr"] = "/uptr/0";

    auto data = fon::yaml::deserialize<B>(node);
    REQUIRE(data.uptr);
    REQUIRE(*data.uptr == 43);
    REQUIRE(data.ptr == data.uptr.get());
}

struct Base {
    int32_t x;

    static void reflect(auto& visitor, auto& self) {
        visitor["x"] % self.x;
    }
};
struct Derived : Base {
    int32_t y;
    Base* self;

    static void reflect(auto& visitor, auto& self) {
        Base::reflect(visitor, self);
        visitor["y"]    % self.y;
        visitor["self"] % self.self;
    }
};

TEST_CASE("test yaml serialization of base pointers", "[yaml][raw][base][pointer][serialize][struct]") {
    auto data = std::make_unique<Derived>();
    data->x = 5;
    data->y = 6;
    data->self = data.get();

    auto node = fon::yaml::serialize(data);
    REQUIRE(node.IsSequence());
    REQUIRE(node[0].IsMap());
    CHECK(node[0]["x"].as<int32_t>() == 5);
    CHECK(node[0]["y"].as<int32_t>() == 6);
    CHECK(node[0]["self"].as<std::string>() == "/0");
}

TEST_CASE("test yaml deserialization of base pointers", "[yaml][raw][base][pointer][deserialize][struct]") {
    YAML::Node node;
    node[0]["x"] = 7;
    node[0]["y"] = 8;
    node[0]["self"] = "/0";

    auto data = fon::yaml::deserialize<std::unique_ptr<Derived>>(node);
    REQUIRE(data != nullptr);
    CHECK(data->x == 7);
    CHECK(data->y == 8);
    CHECK(data->self == data.get());
}

}
