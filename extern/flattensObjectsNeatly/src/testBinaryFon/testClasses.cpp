#include <catch2/catch.hpp>
#include <fon/binary.h>
#include <fon/std/all.h>

namespace {
struct A {
    std::vector<int32_t> xs;

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["xs"] % self.xs;
    }
};

}


TEST_CASE("test binary serialization of struct A", "[binary][struct][serialize]") {
    auto data = A{{10, 20, 30}};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "xs".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},  std::byte{'s'},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of struct A", "[binary][struct][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "xs".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},  std::byte{'s'},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
    };

    auto data = fon::binary::deserialize<A>(input);

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

TEST_CASE("test binary serialization of maps with structs", "[binary][struct][std][map][serialize]") {
    auto data = B{};
    data.infos["k1"].x = 10;
    data.infos["k2"].x = 20;

    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "infos".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'i'},  std::byte{'n'},  std::byte{'f'},  std::byte{'o'},  // "infos"
        std::byte{'s'},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // B::infos[0].key
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].value."x".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},                                                     // B::infos[0].value."x"
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].value.value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // B::infos[1].key
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].value."x".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},                                                     // B::infos[1].value."x"
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].value.value
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of maps with structs", "[binary][struct][std][map][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "infos".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'i'},  std::byte{'n'},  std::byte{'f'},  std::byte{'o'},  // "infos"
        std::byte{'s'},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // B::infos[0].key
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].value."x".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},                                                     // B::infos[0].value."x"
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[0].value.value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // B::infos[1].key
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].value."x".size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},                                                     // B::infos[1].value."x"
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B::infos[1].value.value
    };

    auto data = fon::binary::deserialize<B>(input);

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


TEST_CASE("test binary serialization of struct D (no name)", "[binary][struct][serialize][direct]") {
    auto data = D{{10, 20, 30}};
    auto buffer = fon::binary::serialize(data);
    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
    };
    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of struct D (no name)", "[binary][struct][deserialize][direct]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
    };

    auto data = fon::binary::deserialize<D>(input);
    REQUIRE(data.xs == (std::vector<int32_t>{10, 20, 30}));
}
