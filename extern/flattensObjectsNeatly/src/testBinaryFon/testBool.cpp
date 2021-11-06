#include <catch2/catch.hpp>
#include <fon/binary.h>

TEST_CASE("test binary serialization of bool", "[binary][bool][serialize]") {
    SECTION ("testing 'true'") {
        bool data {true};
        auto buffer = fon::binary::serialize(data);

        auto expected = std::vector<std::byte> {
            std::byte{1},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION ("testing 'false'") {
        bool data {false};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of bool", "[binary][bool][deserialize]") {
    SECTION ("testing 'true'") {
        auto input = std::vector<std::byte> {
            std::byte{1},
        };
        auto output = fon::binary::deserialize<bool>(input);
        REQUIRE(output == true);
    }

    SECTION ("testing 'false'") {
        auto input = std::vector<std::byte> {
            std::byte{0},
        };
        auto output = fon::binary::deserialize<bool>(input);
        REQUIRE(output == false);
    }
}
