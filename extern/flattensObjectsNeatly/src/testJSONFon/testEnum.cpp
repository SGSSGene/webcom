#include <catch2/catch.hpp>
#include <fon/json.h>
#include <fon/std/all.h>

namespace {
enum class MyEnum : int32_t {
    E1,
    E2,
    E3
};

}


TEST_CASE("test json serialization of enum", "[json][enum][serialize]") {
    SECTION("E1") {
        auto data = MyEnum::E1;
        auto node = fon::json::serialize(data);
        INFO(node.asString());
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == int32_t(MyEnum::E1));
    }
    SECTION("E2") {
        auto data = MyEnum::E2;
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == int32_t(MyEnum::E2));
    }
    SECTION("E3") {
        auto data = MyEnum::E3;
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == int32_t(MyEnum::E3));
    }
}
TEST_CASE("test json deserialization of enum", "[json][enum][deserialize]") {
    SECTION("E1") {
        Json::Value node;
        node = int(MyEnum::E1);
        auto data = fon::json::deserialize<MyEnum>(node);
        REQUIRE(data == MyEnum::E1);
    }
    SECTION("E2") {
        Json::Value node;
        node = int(MyEnum::E2);
        auto data = fon::json::deserialize<MyEnum>(node);
        REQUIRE(data == MyEnum::E2);
    }
    SECTION("E3") {
        Json::Value node;
        node = int(MyEnum::E3);
        auto data = fon::json::deserialize<MyEnum>(node);
        REQUIRE(data == MyEnum::E3);
    }
}
