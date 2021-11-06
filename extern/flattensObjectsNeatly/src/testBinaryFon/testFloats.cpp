#include <catch2/catch.hpp>
#include <fon/binary.h>

TEST_CASE("test binary serialization of float", "[binary][float][serialize]") {
    SECTION("positive number") {
        float data {1.34}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte>{
            std::byte{0x1f}, std::byte{0x85}, std::byte{0xab}, std::byte{0x3f},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        float data {-0.45}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte>{
            std::byte{0x66}, std::byte{0x66}, std::byte{0xe6}, std::byte{0xbe},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of float", "[binary][float][deserialize]") {
    SECTION("positive number") {
        // "random number"
        auto input = std::vector<std::byte>{
            std::byte{0x1f}, std::byte{0x85}, std::byte{0xab}, std::byte{0x3f},
        };
        auto data = fon::binary::deserialize<float>(input);
        REQUIRE(data == 1.34f);
    }
    SECTION("small number") {
        // "random number"
        auto input = std::vector<std::byte>{
            std::byte{0x66}, std::byte{0x66}, std::byte{0xe6}, std::byte{0xbe},
        };
        auto data = fon::binary::deserialize<float>(input);
        REQUIRE(data == -0.45f);
    }
}
TEST_CASE("test binary serialization of double", "[binary][double][serialize]") {
    SECTION("positive number") {
        double data {1.34}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte>{
            std::byte{0x71}, std::byte{0x3d}, std::byte{0x0a}, std::byte{0xd7},
            std::byte{0xa3}, std::byte{0x70}, std::byte{0xf5}, std::byte{0x3f},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        double data {-0.45}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte>{
            std::byte{0xcd}, std::byte{0xcc}, std::byte{0xcc}, std::byte{0xcc},
            std::byte{0xcc}, std::byte{0xcc}, std::byte{0xdc}, std::byte{0xbf},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of double", "[binary][double][deserialize]") {
    SECTION("positive number") {
        // "random number"
        auto input = std::vector<std::byte>{
            std::byte{0x71}, std::byte{0x3d}, std::byte{0x0a}, std::byte{0xd7},
            std::byte{0xa3}, std::byte{0x70}, std::byte{0xf5}, std::byte{0x3f},
        };
        auto data = fon::binary::deserialize<double>(input);
        REQUIRE(data == 1.34);
    }
    SECTION("small number") {
        // "random number"
        auto input = std::vector<std::byte>{
            std::byte{0xcd}, std::byte{0xcc}, std::byte{0xcc}, std::byte{0xcc},
            std::byte{0xcc}, std::byte{0xcc}, std::byte{0xdc}, std::byte{0xbf},
        };
        auto data = fon::binary::deserialize<double>(input);
        REQUIRE(data == -0.45);
    }
}
