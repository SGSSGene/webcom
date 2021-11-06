#include <catch2/catch.hpp>
#include <fon/binary.h>

TEST_CASE("test binary serialization of int8_t", "[binary][int8_t][serialize]") {
    SECTION("positive number") {
        int8_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        int8_t data {-7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{249},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        int8_t data {127};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{127},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        int8_t data {-128};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{128},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}
TEST_CASE("test binary deserialization of int8_t", "[binary][int8_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, // "random" number
        };
        auto data = fon::binary::deserialize<int8_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("small number") {
        auto input = std::vector<std::byte> {
            std::byte{249}, // "random" number
        };
        auto data = fon::binary::deserialize<int8_t>(input);
        REQUIRE(data == -7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{127},
        };
        auto data = fon::binary::deserialize<int8_t>(input);
        REQUIRE(data == 127);
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{128},
        };
        auto data = fon::binary::deserialize<int8_t>(input);
        REQUIRE(data == -128);
    }
}
TEST_CASE("test binary serialization of uint8_t", "[binary][uint8_t][serialize]") {
    SECTION("positive number") {
        uint8_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        uint8_t data {255};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        uint8_t data {0};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of uint8_t", "[binary][uint8_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, // "random" number
        };
        auto data = fon::binary::deserialize<uint8_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255},
        };
        auto data = fon::binary::deserialize<uint8_t>(input);
        REQUIRE(data == 255);
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0},
        };
        auto data = fon::binary::deserialize<uint8_t>(input);
        REQUIRE(data == 0);
    }
}


TEST_CASE("test binary serialization of int16_t", "[binary][int16_t][serialize]") {
    SECTION("positive number") {
        int16_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        int16_t data {-7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{249}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        int16_t data {32767};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{127},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        int16_t data {-32768};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{128},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of int16_t", "[binary][int16_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, // "random" number
        };
        auto data = fon::binary::deserialize<int16_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("small number") {
        auto input = std::vector<std::byte> {
            std::byte{249}, std::byte{255}, // "random" number
        };
        auto data = fon::binary::deserialize<int16_t>(input);
        REQUIRE(data == -7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{127},
        };
        auto data = fon::binary::deserialize<int16_t>(input);
        REQUIRE(data == 32767);
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{128},
        };
        auto data = fon::binary::deserialize<int16_t>(input);
        REQUIRE(data == -32768);
    }
}

TEST_CASE("test binary serialization of uint16_t", "[binary][uint16_t][serialize]") {
    SECTION("positive number") {
        uint16_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        uint16_t data {65535};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        uint16_t data {0};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of uint16_t", "[binary][uint16_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, // "random" number
        };
        auto data = fon::binary::deserialize<uint16_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{255},
        };
        auto data = fon::binary::deserialize<uint16_t>(input);
        REQUIRE(data == 65535);
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<uint16_t>(input);
        REQUIRE(data == 0);
    }
}

TEST_CASE("test binary serialization of int32_t", "[binary][int32_t][serialize]") {
    SECTION("positive number") {
        int32_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        int32_t data {-7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{249}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        int32_t data {std::numeric_limits<int32_t>::max()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{127},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        int32_t data {std::numeric_limits<int32_t>::min()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{128},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of int32_t", "[binary][int32_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0}, // "random" number
        };
        auto data = fon::binary::deserialize<int32_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("small number") {
        auto input = std::vector<std::byte> {
            std::byte{249}, std::byte{255}, std::byte{255}, std::byte{255}, // "random" number
        };
        auto data = fon::binary::deserialize<int32_t>(input);
        REQUIRE(data == -7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{127},
        };
        auto data = fon::binary::deserialize<int32_t>(input);
        REQUIRE(data == std::numeric_limits<int32_t>::max());
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{128},
        };
        auto data = fon::binary::deserialize<int32_t>(input);
        REQUIRE(data == std::numeric_limits<int32_t>::min());
    }
}

TEST_CASE("test binary serialization of uint32_t", "[binary][uint32_t][serialize]") {
    SECTION("positive number") {
        uint32_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0}
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        uint32_t data {std::numeric_limits<uint32_t>::max()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        uint32_t data {std::numeric_limits<uint32_t>::min()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of uint32_t", "[binary][uint32_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0}, // "random" number
        };
        auto data = fon::binary::deserialize<uint32_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        auto data = fon::binary::deserialize<uint32_t>(input);
        REQUIRE(data == std::numeric_limits<uint32_t>::max());
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0}
        };
        auto data = fon::binary::deserialize<uint32_t>(input);
        REQUIRE(data == std::numeric_limits<uint32_t>::min());
    }
}

TEST_CASE("test binary serialization of int64_t", "[binary][int64_t][serialize]") {
    SECTION("positive number") {
        int64_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("small number") {
        int64_t data {-7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{249}, std::byte{255}, std::byte{255}, std::byte{255},
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        int64_t data {std::numeric_limits<int64_t>::max()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{127},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        int64_t data {std::numeric_limits<int64_t>::min()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{128},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of int64_t", "[binary][int64_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0}, // "random" number
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<int64_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("small number") {
        auto input = std::vector<std::byte> {
            std::byte{249}, std::byte{255}, std::byte{255}, std::byte{255}, // "random" number
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        auto data = fon::binary::deserialize<int64_t>(input);
        REQUIRE(data == -7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{127},
        };
        auto data = fon::binary::deserialize<int64_t>(input);
        REQUIRE(data == std::numeric_limits<int64_t>::max());
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{128},
        };
        auto data = fon::binary::deserialize<int64_t>(input);
        REQUIRE(data == std::numeric_limits<int64_t>::min());
    }
}

TEST_CASE("test binary serialization of uint64_t", "[binary][uint64_t][serialize]") {
    SECTION("positive number") {
        uint64_t data {7}; // "random" number
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("max number") {
        uint64_t data {std::numeric_limits<uint64_t>::max()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }

    SECTION("min number") {
        uint64_t data {std::numeric_limits<uint64_t>::min()};
        auto buffer = fon::binary::serialize(data);
        auto expected = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        REQUIRE(buffer.size() == expected.size());
        CHECK(buffer == expected);
    }
}

TEST_CASE("test binary deserialization of uint64_t", "[binary][uint64_t][deserialize]") {
    SECTION("positive number") {
        auto input = std::vector<std::byte> {
            std::byte{7}, std::byte{0}, std::byte{0}, std::byte{0}, // "random" number
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<uint64_t>(input);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        auto input = std::vector<std::byte> {
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
            std::byte{255}, std::byte{255}, std::byte{255}, std::byte{255},
        };
        auto data = fon::binary::deserialize<uint64_t>(input);
        REQUIRE(data == std::numeric_limits<uint64_t>::max());
    }
    SECTION("min number") {
        auto input = std::vector<std::byte> {
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
            std::byte{0}, std::byte{0}, std::byte{0}, std::byte{0},
        };
        auto data = fon::binary::deserialize<uint64_t>(input);
        REQUIRE(data == std::numeric_limits<uint64_t>::min());
    }
}
