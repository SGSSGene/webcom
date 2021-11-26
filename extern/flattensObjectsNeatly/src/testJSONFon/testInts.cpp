#include <catch2/catch.hpp>
#include <fon/json.h>

TEST_CASE("test json serialization of int8_t", "[json][int8_t][serialize]") {
    SECTION("positive number") {
        int8_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == 7);
    }

    SECTION("small number") {
        int8_t data {-7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == -7);
    }

    SECTION("max number") {
        int8_t data {127};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == 127);
    }

    SECTION("min number") {
        int8_t data {-128};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == -128);
    }

}

TEST_CASE("test json deserialization of int8_t", "[json][int8_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<int8_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("small number") {
        Json::Value node;
        node = -7; // "random" number
        auto data = fon::json::deserialize<int8_t>(node);
        REQUIRE(data == -7);
    }
    SECTION("max number") {
        Json::Value node;
        node = 127;
        auto data = fon::json::deserialize<int8_t>(node);
        REQUIRE(data == 127);
    }
    SECTION("min number") {
        Json::Value node;
        node = -128;
        auto data = fon::json::deserialize<int8_t>(node);
        REQUIRE(data == -128);
    }

    SECTION("number above range") {
        Json::Value node;
        node = 128;
        REQUIRE_THROWS(fon::json::deserialize<int8_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = -129;
        REQUIRE_THROWS(fon::json::deserialize<int8_t>(node));
    }
}
TEST_CASE("test json serialization of uint8_t", "[json][uint8_t][serialize]") {
    SECTION("positive number") {
        uint8_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == 7);
    }

    SECTION("max number") {
        uint8_t data {255};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == 255);
    }

    SECTION("min number") {
        uint8_t data {0};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == 0);
    }
}

TEST_CASE("test json deserialization of uint8_t", "[json][uint8_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<uint8_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = 255;
        auto data = fon::json::deserialize<uint8_t>(node);
        REQUIRE(data == 255);
    }
    SECTION("min number") {
        Json::Value node;
        node = 0;
        auto data = fon::json::deserialize<uint8_t>(node);
        REQUIRE(data == 0);
    }

    SECTION("number above range") {
        Json::Value node;
        node = 256;
        REQUIRE_THROWS(fon::json::deserialize<uint8_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = -1;
        REQUIRE_THROWS(fon::json::deserialize<uint8_t>(node));
    }
}

TEST_CASE("test json serialization of int16_t", "[json][int16_t][serialize]") {
    SECTION("positive number") {
        int16_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == 7);
    }

    SECTION("max number") {
        int16_t data {std::numeric_limits<int16_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == std::numeric_limits<int16_t>::max());
    }

    SECTION("min number") {
        int16_t data {std::numeric_limits<int16_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == std::numeric_limits<int16_t>::min());
    }
}

TEST_CASE("test json deserialization of int16_t", "[json][int16_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<int16_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<int16_t>::max();
        auto data = fon::json::deserialize<int16_t>(node);
        REQUIRE(data == std::numeric_limits<int16_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<int16_t>::min();
        auto data = fon::json::deserialize<int16_t>(node);
        REQUIRE(data == std::numeric_limits<int16_t>::min());
    }

    SECTION("number above range") {
        Json::Value node;
        node = std::numeric_limits<int16_t>::max() + 1;
        REQUIRE_THROWS(fon::json::deserialize<int16_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = std::numeric_limits<int16_t>::min() - 1;
        REQUIRE_THROWS(fon::json::deserialize<int16_t>(node));
    }
}

TEST_CASE("test json serialization of uint16_t", "[json][uint16_t][serialize]") {
    SECTION("positive number") {
        uint16_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == 7);
    }

    SECTION("max number") {
        uint16_t data {std::numeric_limits<uint16_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == std::numeric_limits<uint16_t>::max());
    }

    SECTION("min number") {
        uint16_t data {std::numeric_limits<uint16_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == std::numeric_limits<uint16_t>::min());
    }
}

TEST_CASE("test json deserialization of uint16_t", "[json][uint16_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<uint16_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<uint16_t>::max();
        auto data = fon::json::deserialize<uint16_t>(node);
        REQUIRE(data == std::numeric_limits<uint16_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<uint16_t>::min();
        auto data = fon::json::deserialize<uint16_t>(node);
        REQUIRE(data == std::numeric_limits<uint16_t>::min());
    }

    SECTION("number above range") {
        Json::Value node;
        node = std::numeric_limits<uint16_t>::max() + 1;
        REQUIRE_THROWS(fon::json::deserialize<uint16_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = -1;
//        auto data = fon::json::deserialize<uint16_t>(node);
//        std::cout << data << "\n";
        REQUIRE_THROWS(fon::json::deserialize<uint16_t>(node));
    }
}
TEST_CASE("test json serialization of int32_t", "[json][int32_t][serialize]") {
    SECTION("positive number") {
        int32_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == 7);
    }

    SECTION("max number") {
        int32_t data {std::numeric_limits<int32_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == std::numeric_limits<int32_t>::max());
    }

    SECTION("min number") {
        int32_t data {std::numeric_limits<int32_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == std::numeric_limits<int32_t>::min());
    }
}

TEST_CASE("test json deserialization of int32_t", "[json][int32_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<int32_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<int32_t>::max();
        auto data = fon::json::deserialize<int32_t>(node);
        REQUIRE(data == std::numeric_limits<int32_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<int32_t>::min();
        auto data = fon::json::deserialize<int32_t>(node);
        REQUIRE(data == std::numeric_limits<int32_t>::min());
    }

    SECTION("number above range") {
        Json::Value node;
        node = std::numeric_limits<int32_t>::max() + int64_t{1};
        REQUIRE_THROWS(fon::json::deserialize<int32_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = std::numeric_limits<int32_t>::min() - int64_t{1};
        REQUIRE_THROWS(fon::json::deserialize<int32_t>(node));
    }
}

TEST_CASE("test json serialization of uint32_t", "[json][uint32_t][serialize]") {
    SECTION("positive number") {
        uint32_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == 7);
    }

    SECTION("max number") {
        uint32_t data {std::numeric_limits<uint32_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == std::numeric_limits<uint32_t>::max());
    }

    SECTION("min number") {
        uint32_t data {std::numeric_limits<uint32_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt());
        REQUIRE(node.asUInt() == std::numeric_limits<uint32_t>::min());
    }
}

TEST_CASE("test json deserialization of uint32_t", "[json][uint32_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<uint32_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<uint32_t>::max();
        auto data = fon::json::deserialize<uint32_t>(node);
        REQUIRE(data == std::numeric_limits<uint32_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<uint32_t>::min();
        auto data = fon::json::deserialize<uint32_t>(node);
        REQUIRE(data == std::numeric_limits<uint32_t>::min());
    }

    SECTION("number above range") {
        Json::Value node;
        node = std::numeric_limits<uint32_t>::max() + uint64_t{1};
        REQUIRE_THROWS(fon::json::deserialize<uint32_t>(node));
    }
    SECTION("number below range") {
        Json::Value node;
        node = int64_t{std::numeric_limits<uint32_t>::min()} - int64_t{1};
        REQUIRE_THROWS(fon::json::deserialize<uint32_t>(node));
    }
}
TEST_CASE("test json serialization of int64_t", "[json][int64_t][serialize]") {
    SECTION("positive number") {
        int64_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt());
        REQUIRE(node.asInt() == 7);
    }

    SECTION("max number") {
        int64_t data {std::numeric_limits<int64_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt64());
        REQUIRE(node.asInt64() == std::numeric_limits<int64_t>::max());
    }

    SECTION("min number") {
        int64_t data {std::numeric_limits<int64_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isInt64());
        REQUIRE(node.asInt64() == std::numeric_limits<int64_t>::min());
    }
}

TEST_CASE("test json deserialization of int64_t", "[json][int64_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<int64_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<int64_t>::max();
        auto data = fon::json::deserialize<int64_t>(node);
        REQUIRE(data == std::numeric_limits<int64_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<int64_t>::min();
        auto data = fon::json::deserialize<int64_t>(node);
        REQUIRE(data == std::numeric_limits<int64_t>::min());
    }
}

TEST_CASE("test json serialization of uint64_t", "[json][uint64_t][serialize]") {
    SECTION("positive number") {
        uint64_t data {7}; // "random" number
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt64());
        REQUIRE(node.asUInt64() == 7);
    }

    SECTION("max number") {
        uint64_t data {std::numeric_limits<uint64_t>::max()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt64());
        REQUIRE(node.asUInt64() == std::numeric_limits<uint64_t>::max());
    }

    SECTION("min number") {
        uint64_t data {std::numeric_limits<uint64_t>::min()};
        auto node = fon::json::serialize(data);
        REQUIRE(node.isUInt64());
        REQUIRE(node.asUInt64() == std::numeric_limits<uint64_t>::min());
    }
}

TEST_CASE("test json deserialization of uint64_t", "[json][uint64_t][deserialize]") {
    SECTION("positive number") {
        Json::Value node;
        node = 7; // "random" number
        auto data = fon::json::deserialize<uint64_t>(node);
        REQUIRE(data == 7);
    }
    SECTION("max number") {
        Json::Value node;
        node = std::numeric_limits<uint64_t>::max();
        auto data = fon::json::deserialize<uint64_t>(node);
        REQUIRE(data == std::numeric_limits<uint64_t>::max());
    }
    SECTION("min number") {
        Json::Value node;
        node = std::numeric_limits<uint64_t>::min();
        auto data = fon::json::deserialize<uint64_t>(node);
        REQUIRE(data == std::numeric_limits<uint64_t>::min());
    }
}
