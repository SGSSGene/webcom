#include <catch2/catch.hpp>
#include <fon/json.h>

TEST_CASE("test json serialization of float", "[json][float][serialize]") {
    SECTION("positive number") {
        float data {1.34}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isDouble());
        REQUIRE(node.asFloat() == 1.34f);
    }

    SECTION("small number") {
        float data {-0.45}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isDouble());
        REQUIRE(node.asFloat() == -0.45f);
    }
}

TEST_CASE("test json deserialization of float", "[json][float][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 1.34; // "random" number
        auto data = fon::json::deserialize<float>(node);
        REQUIRE(data == 1.34f);
    }
    SECTION("small number") {
        Json::Value node;
        node = -0.45; // "random" number
        auto data = fon::json::deserialize<float>(node);
        REQUIRE(data == -0.45f);
    }
}
TEST_CASE("test json serialization of double", "[json][double][serialize]") {
    SECTION("positive number") {
        double data {1.34}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isDouble());
        REQUIRE(node.asDouble() == 1.34);
    }

    SECTION("small number") {
        double data {-0.45}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isDouble());
        REQUIRE(node.asDouble() == -0.45);
    }
}

TEST_CASE("test json deserialization of double", "[json][double][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 1.34; // "random" number
        auto data = fon::json::deserialize<double>(node);
        REQUIRE(data == 1.34);
    }
    SECTION("small number") {
        Json::Value node;
        node = -0.45; // "random" number
        auto data = fon::json::deserialize<double>(node);
        REQUIRE(data == -0.45);
    }
}
