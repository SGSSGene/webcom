#include <catch2/catch.hpp>

#include <fon/json.h>
#include <fon/std/all.h>

namespace {

struct B {
    Json::Value node;

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["node"] % self.node;
    }
};

TEST_CASE("test json serialization of Json::Value", "[json][serialize][json-cpp]") {
    auto data = B{};
    data.node["foo"] = "bar";

    auto node = fon::json::serialize(data);

    REQUIRE(node.isObject());
    REQUIRE(node["node"].isObject());
    CHECK(node["node"]["foo"].asString() == "bar");
}

TEST_CASE("test json deserialization of Json::Value", "[json][deserialize][json-cpp]") {
    Json::Value node;
    node["node"]["foo"] = "bar";

    auto data = fon::json::deserialize<B>(node);
    REQUIRE(data.node.isObject());
    CHECK(data.node["foo"].asString() == "bar");
}

TEST_CASE("test json deserialization of Json::Value inside tuples", "[json][deserialize][json-cpp][tuple]") {
    Json::Value node;
    node["0"] = "bar";

    auto data = fon::json::deserialize<std::tuple<Json::Value>>(node);
    REQUIRE(std::get<0>(data).isString());
    CHECK(std::get<0>(data).asString() == "bar");
}

}
