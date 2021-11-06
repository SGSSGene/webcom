#include <catch2/catch.hpp>
#include <fon/binary.h>
#include <fon/std/all.h>

namespace {
enum class MyEnum : int32_t {
    E1,
    E2,
    E3
};

}

TEST_CASE("test binary serialization of enum", "[binary][enum][serialize]") {
    SECTION("E1") {
        auto data = MyEnum::E1;
        auto buffer = fon::binary::serialize(data);

        auto expected = std::vector<std::byte> {
            std::byte(MyEnum::E1), std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
    SECTION("E2") {
        auto data = MyEnum::E2;
        auto buffer = fon::binary::serialize(data);

        auto expected = std::vector<std::byte> {
            std::byte(MyEnum::E2), std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
    SECTION("E3") {
        auto data = MyEnum::E3;
        auto buffer = fon::binary::serialize(data);

        auto expected = std::vector<std::byte> {
            std::byte(MyEnum::E3), std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}
TEST_CASE("test binary deserialization of enum", "[binary][enum][deserialize]") {
    SECTION("E1") {
        auto input= std::vector<std::byte> {
            std::byte(MyEnum::E1), std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<MyEnum>(input);
        REQUIRE(data == MyEnum::E1);
    }
    SECTION("E2") {
        auto input = std::vector<std::byte> {
            std::byte(MyEnum::E2), std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<MyEnum>(input);
        REQUIRE(data == MyEnum::E2);
    }
    SECTION("E3") {
        auto input = std::vector<std::byte> {
            std::byte(MyEnum::E3), std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<MyEnum>(input);
        REQUIRE(data == MyEnum::E3);
    }
}
