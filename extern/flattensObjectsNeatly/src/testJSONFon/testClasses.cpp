#include <catch2/catch.hpp>
#include <fon/json.h>
#include <fon/std/all.h>

namespace {
struct A {
    std::vector<int32_t> xs;

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["xs"] % self.xs;
    }
};

}

TEST_CASE("test json serialization of struct A", "[json][struct][serialize]") {
    auto data = A{{10, 20, 30}};
    auto node = fon::json::serialize(data);

    INFO(node.toStyledString());

    REQUIRE(node.isObject());
    REQUIRE(node["xs"].isArray());
    REQUIRE(node["xs"].size() == 3);
    REQUIRE(node["xs"][0].asInt() == 10);
    REQUIRE(node["xs"][1].asInt() == 20);
    REQUIRE(node["xs"][2].asInt() == 30);
}

TEST_CASE("test json deserialization of struct A", "[json][struct][deserialize]") {
    Json::Value node;
    node["xs"][0] = 10;
    node["xs"][1] = 20;
    node["xs"][2] = 30;
    auto data = fon::json::deserialize<A>(node);
    REQUIRE(data.xs.size() == 3);
    REQUIRE(data.xs == (std::vector<int32_t>{10, 20, 30}));
}

namespace {
struct B {
    struct C {
        int32_t x;

        constexpr static void reflect(auto& visitor, auto& self) {
            visitor["x"] % self.x;
        }
    };
    std::map<std::string, C> infos;

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["infos"] % self.infos;
    }
};
}

TEST_CASE("test json serialization of maps with structs", "[json][struct][std][map][serialize]") {
    auto data = B{};
    data.infos["k1"].x = 10;
    data.infos["k2"].x = 20;

    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 1);
    REQUIRE(node["infos"].isObject());
    REQUIRE(node["infos"].size() == 2);
    REQUIRE(node["infos"]["k1"]["x"].as<int>() == 10);
    REQUIRE(node["infos"]["k2"]["x"].as<int>() == 20);
}

TEST_CASE("test json deserialization of maps with structs", "[json][struct][std][map][deserialize]") {
    Json::Value node;
    node["infos"]["k1"]["x"] = 10;
    node["infos"]["k2"]["x"] = 20;

    REQUIRE(node.isObject());
    REQUIRE(node["infos"].isObject());
    REQUIRE(node["infos"]["k1"].isObject());
    REQUIRE(node["infos"]["k2"].isObject());
    REQUIRE(node["infos"]["k1"]["x"].isInt());
    REQUIRE(node["infos"]["k2"]["x"].isInt());

    auto data = fon::json::deserialize<B>(node);

    REQUIRE(data.infos.size() == 2);
    REQUIRE(data.infos.find("k1") != data.infos.end());
    REQUIRE(data.infos.find("k2") != data.infos.end());
    REQUIRE(data.infos.at("k1").x == 10);
    REQUIRE(data.infos.at("k2").x == 20);
}

namespace {
struct D {
    std::vector<int32_t> xs;

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor % self.xs;
    }
};
}

TEST_CASE("test json serialization of struct D (no name)", "[json][struct][serialize][direct]") {
    auto data = D{{10, 20, 30}};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}

TEST_CASE("test json deserialization of struct D (no name)", "[json][struct][deserialize][direct]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<D>(node);
    REQUIRE(data.xs == (std::vector<int32_t>{10, 20, 30}));
}
