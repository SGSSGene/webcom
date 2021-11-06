#include <catch2/catch.hpp>
#include <fon/binary.h>
#include <fon/std/all.h>

TEST_CASE("test binary serialization of std::string", "[yaml][std][string][serialize]") {
    auto data = std::string{"hallo welt"};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'}, // actual string
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::string", "[yaml][std][string][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'}, // actual string
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    auto data = fon::binary::deserialize<std::string>(input);
    auto expected = std::string{"hallo welt"};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::vector", "[yaml][std][vector][serialize]") {
    auto data = std::vector<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::vector", "[yaml][std][vector][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[2]
    };
    auto data = fon::binary::deserialize<std::vector<int32_t>>(input);
    auto expected = std::vector<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::vector with strings", "[yaml][std][vector][string][serialize]") {
    auto data = std::vector<std::string>{"hallo", "welt", "!"};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // string[0]
        std::byte{'o'},
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[1].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'w'},  std::byte{'e'},  std::byte{'l'},  std::byte{'t'},  // string[1]
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[2].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'!'},                                                     // string[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::vector with strings", "[yaml][std][vector][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // string[0]
        std::byte{'o'},
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[1].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'w'},  std::byte{'e'},  std::byte{'l'},  std::byte{'t'},  // string[1]
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string[2].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'!'},                                                     // string[2]
    };

    auto data = fon::binary::deserialize<std::vector<std::string>>(input);
    auto expected = std::vector<std::string>{"hallo", "welt", "!"};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::array", "[yaml][std][array][serialize]") {
    auto data = std::array<int32_t, 3>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::array", "[yaml][std][array][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // array[2]
    };
    auto data = fon::binary::deserialize<std::array<int32_t, 3>>(input);
    auto expected = std::array<int32_t, 3>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::list", "[yaml][std][list][serialize]") {
    auto data = std::list<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::list", "[yaml][std][list][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };
    auto data = fon::binary::deserialize<std::list<int32_t>>(input);
    auto expected = std::list<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::forward_list", "[yaml][std][forward_list][serialize]") {
    auto data = std::forward_list<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::forward_list", "[yaml][std][forward_list][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };
    auto data = fon::binary::deserialize<std::forward_list<int32_t>>(input);
    auto expected = std::forward_list<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::deque", "[yaml][std][deque][serialize]") {
    auto data = std::deque<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::deque", "[yaml][std][deque][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };
    auto data = fon::binary::deserialize<std::deque<int32_t>>(input);
    auto expected = std::deque<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::set", "[yaml][std][set][serialize]") {
    auto data = std::set<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::set", "[yaml][std][set][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };
    auto data = fon::binary::deserialize<std::set<int32_t>>(input);
    auto expected = std::set<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::unordered_set", "[yaml][std][unordered_set][serialize]") {
    auto data = std::unordered_set<int32_t>{10, 20, 30};
    auto buffer = fon::binary::serialize(data);

    REQUIRE(buffer.size() == 20);
    CHECK(buffer[0] == std::byte{0x03});
    for (auto i : {1, 2, 3, 4, 5, 6, 7, 9, 10, 11, 13, 14, 15, 17, 18, 19}) {
        CHECK(buffer[i] == std::byte{0});
    }
    CHECK((std::set{buffer[8], buffer[12], buffer[16]}) == (std::set{std::byte{10}, std::byte{20}, std::byte{30}}));
}

TEST_CASE("test binary deserialization of std::unordered_set", "[yaml][std][unordered_set][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // vector size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[2]
    };
    auto data = fon::binary::deserialize<std::unordered_set<int32_t>>(input);
    auto expected = std::unordered_set<int32_t>{10, 20, 30};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::map", "[yaml][std][map][serialize]") {
    auto data = std::map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // map[1].key
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'3'},                                    // map[2].key
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].value
    };
    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}
TEST_CASE("test binary deserialization of std::map", "[yaml][std][map][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // map[1].key
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'3'},                                    // map[2].key
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].value
    };
    auto data = fon::binary::deserialize<std::map<std::string, int32_t>>(input);
    REQUIRE(data == (std::map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}}));
}

TEST_CASE("test binary serialization of std::unordered_map", "[yaml][std][unordered_map][serialize]") {
    auto data = std::unordered_map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}};
    auto buffer = fon::binary::serialize(data);

    auto expectedHead = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    auto expectedEntryK1 = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value
    };

    auto expectedEntryK2 = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // map[1].key
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
    };

    auto expectedEntryK3 = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'3'},                                    // map[2].key
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].value
    };

    REQUIRE(buffer.size() == expectedHead.size() + expectedEntryK1.size()
                           + expectedEntryK2.size() + expectedEntryK3.size());
    auto cut = [&](size_t idx, size_t len) {
        return std::vector<std::byte>{
            begin(buffer) + idx,
            begin(buffer) + idx + len
        };
    };

    CHECK(cut(0, 8) == expectedHead);
    CHECK((std::set{cut(8, 14), cut(22, 14), cut(36, 14)})
          == (std::set{expectedEntryK1, expectedEntryK2, expectedEntryK3}));
}

TEST_CASE("test binary deserialization of std::unordered_map", "[yaml][std][unordered_map][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'1'},                                    // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'2'},                                    // map[1].key
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].key.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'k'},  std::byte{'3'},                                    // map[2].key
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[2].value
    };

    auto data = fon::binary::deserialize<std::unordered_map<std::string, int32_t>>(input);
    REQUIRE(data == (std::unordered_map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}}));
}

TEST_CASE("test binary serialization of std::pair", "[yaml][std][pair][serialize]") {
    auto data = std::pair<std::string, int32_t>{"hallo welt", 42};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // map[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
    };
    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::pair", "[yaml][std][pair][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // map[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].key
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
    };

    auto data = fon::binary::deserialize<std::pair<std::string, int32_t>>(input);
    REQUIRE(data.first == "hallo welt");
    REQUIRE(data.second == 42);
}

TEST_CASE("test binary serialization of std::tuple<>", "[yaml][std][tuple][serialize]") {
    auto data = std::tuple<>{};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{};
    REQUIRE(expected == buffer);
}

TEST_CASE("test binary deserialization of std::tuple<>", "[yaml][std][tuple][deserialize]") {
    auto input = std::vector<std::byte>{};
    auto data = fon::binary::deserialize<std::tuple<>>(input);

    REQUIRE(data == std::tuple<>{});
}

TEST_CASE("test binary serialization of std::tuple<X>", "[yaml][std][tuple][serialize]") {
    auto data = std::tuple<std::string> {"hallo welt"};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // tuple[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of std::tuple<X>", "[yaml][std][tuple][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // tuple[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    auto data = fon::binary::deserialize<std::tuple<std::string>>(input);
    REQUIRE(std::get<0>(data) == "hallo welt");
}

TEST_CASE("test binary serialization of std::tuple<X, Y>", "[yaml][std][tuple][serialize]") {
    auto data = std::tuple<std::string, int32_t>{"hallo welt", 42};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // tuple[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[1].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[1].value
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of std::tuple<X, Y>", "[yaml][std][tuple][deserialize]") {
   auto input = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[0].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // tuple[0].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[1].key
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // tuple[1].value
    };

    auto data = fon::binary::deserialize<std::tuple<std::string, int32_t>>(input);
    REQUIRE(std::get<0>(data) == "hallo welt");
    REQUIRE(std::get<1>(data) == 42);
}

TEST_CASE("test binary serialization of std::optional", "[yaml][std][optional][serialize]") {
    auto data = std::optional<std::string>{"hallo welt"};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // list[0]
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of std::optional", "[yaml][std][optional][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // list[0]
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };

    auto data = fon::binary::deserialize<std::optional<std::string>>(input);
    REQUIRE(data.has_value());
    auto expected = std::string{"hallo welt"};
    CHECK(data.value() == expected);
}

TEST_CASE("test binary serialization of empty std::optional", "[yaml][std][optional][serialize]") {
    auto data = std::optional<std::string>{};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    REQUIRE(buffer == expected);
}
TEST_CASE("test binary deserialization of empty std::optional", "[yaml][std][optional][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    auto data = fon::binary::deserialize<std::optional<std::string>>(input);
    REQUIRE(not data.has_value());
}

TEST_CASE("test binary serialization of std::variant (index 0)", "[yaml][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{std::string{"hallo welt"}};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x00},                                                    // map[1].value
        std::byte{0x01},                                                    // map[1].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // map[1].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };
    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::variant (index 0)", "[yaml][std][variant][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x00},                                                    // map[1].value
        std::byte{0x01},                                                    // map[1].key
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'h'},  std::byte{'a'},  std::byte{'l'},  std::byte{'l'},  // map[1].value
        std::byte{'o'},  std::byte{' '},  std::byte{'w'},  std::byte{'e'},
        std::byte{'l'},  std::byte{'t'},
    };
    auto data = fon::binary::deserialize<std::variant<std::string, int32_t, bool>>(input);

    REQUIRE(data.index() == 0);
    REQUIRE(std::get<0>(data) == "hallo welt");
}

TEST_CASE("test binary serialization of std::variant (index 1)", "[yaml][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{int32_t{42}};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x01},                                                    // map[1].value
        std::byte{0x01},                                                    // map[1].key
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
    };
    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::variant (index 1)", "[yaml][std][variant][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x01},                                                    // map[1].value
        std::byte{0x01},                                                    // map[1].key
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // map[1].value
    };

    auto data = fon::binary::deserialize<std::variant<std::string, int32_t, bool>>(input);
    REQUIRE(data.index() == 1);
    REQUIRE(std::get<1>(data) == 42);
}

TEST_CASE("test binary serialization of std::variant (index 2)", "[yaml][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{true};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x02},                                                    // map[1].value
        std::byte{0x1},                                                     // map[1].key
        std::byte{0x01},                                                    // map[1].value
    };
    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::variant (index 2)", "[yaml][std][variant][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x00},                                                    // map[0].key
        std::byte{0x02},                                                    // map[1].value
        std::byte{0x01},                                                    // map[1].key
        std::byte{0x01},                                                    // map[1].value
    };

    auto data = fon::binary::deserialize<std::variant<std::string, int32_t, bool>>(input);
    REQUIRE(data.index() == 2);
    REQUIRE(std::get<2>(data) == true);
}

TEST_CASE("test binary serialization of std::filesystem::path", "[yaml][std][filesystem][path][serialize]") {
    auto data = std::filesystem::path{"./myfile.txt"};
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x0c}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'.'},  std::byte{'/'},  std::byte{'m'},  std::byte{'y'},  // actual string
        std::byte{'f'},  std::byte{'i'},  std::byte{'l'},  std::byte{'e'},
        std::byte{'.'},  std::byte{'t'},  std::byte{'x'},  std::byte{'t'},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::filesystem::path", "[yaml][std][filesystem][path][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x0b}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // string size
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'.'},  std::byte{'/'},  std::byte{'s'},  std::byte{'o'},  // actual string
        std::byte{'m'},  std::byte{'e'},  std::byte{'_'},  std::byte{'f'},
        std::byte{'i'},  std::byte{'l'},  std::byte{'e'},
    };

    auto data = fon::binary::deserialize<std::filesystem::path>(input);
    auto expected = std::filesystem::path{"./some_file"};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::set<std::filesystem::path>", "[yaml][std][filesystem][set][path][serialize]") {
    auto data = std::set<std::filesystem::path>{{"./myfile.txt"}};

    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0c}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'.'},  std::byte{'/'},  std::byte{'m'},  std::byte{'y'},  // list[0]
        std::byte{'f'},  std::byte{'i'},  std::byte{'l'},  std::byte{'e'},
        std::byte{'.'},  std::byte{'t'},  std::byte{'x'},  std::byte{'t'},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::set<std::filesystem::path>", "[yaml][std][filesystem][set][path][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0c}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // list[0].size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'.'},  std::byte{'/'},  std::byte{'m'},  std::byte{'y'},  // list[0]
        std::byte{'f'},  std::byte{'i'},  std::byte{'l'},  std::byte{'e'},
        std::byte{'.'},  std::byte{'t'},  std::byte{'x'},  std::byte{'t'},
    };

    auto data = fon::binary::deserialize<std::set<std::filesystem::path>>(input);

    auto expected = std::set<std::filesystem::path>{{"./myfile.txt"}};
    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::chrono::time_point<ms>", "[yaml][std][chrono][time_point][serialize]") {
    auto data = std::chrono::time_point<std::chrono::milliseconds>(std::chrono::milliseconds{42});
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::chrono::time_point<ms>", "[yaml][std][chrono][time_point][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };
    auto data = fon::binary::deserialize<std::chrono::time_point<std::chrono::milliseconds>>(input);

    auto expected = std::chrono::time_point<std::chrono::milliseconds>(std::chrono::milliseconds{42});

    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::chrono::time_point<ns>", "[yaml][std][chrono][time_point][serialize]") {
    auto data = std::chrono::time_point<std::chrono::nanoseconds>(std::chrono::nanoseconds{42});
    auto buffer = fon::binary::serialize(data);
    auto expected = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::chrono::time_point<ns>", "[yaml][std][chrono][time_point][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };
    auto data = fon::binary::deserialize<std::chrono::time_point<std::chrono::nanoseconds>>(input);
    auto expected = std::chrono::time_point<std::chrono::nanoseconds>(std::chrono::nanoseconds{42});

    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::filesystem::file_time_type", "[yaml][std][chrono][time_point][serialize]") {
    auto data = std::filesystem::file_time_type(std::chrono::nanoseconds{42});
    auto buffer = fon::binary::serialize(data);
    auto expected = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::filesystem::file_time_type", "[yaml][std][chrono][time_point][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };
    auto data = fon::binary::deserialize<std::filesystem::file_time_type>(input);
    auto expected = std::filesystem::file_time_type(std::chrono::nanoseconds{42});

    REQUIRE(data == expected);
}

TEST_CASE("test binary serialization of std::chrono::duration", "[yaml][std][chrono][duration][serialize]") {
    using namespace std::chrono_literals;
    auto data = 42ms - 20ms;
    auto buffer = fon::binary::serialize(data);
    auto expected = std::vector<std::byte>{
        std::byte{0x16}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    REQUIRE(buffer.size() == expected.size());
    CHECK(buffer == expected);
}

TEST_CASE("test binary deserialization of std::chrono::duration", "[yaml][std][chrono][duration][deserialize]") {
    using namespace std::chrono_literals;

    auto input = std::vector<std::byte>{
        std::byte{0x16}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
    };

    using duration = decltype(42ms - 20ms);
    auto data = fon::binary::deserialize<duration>(input);
    auto expected = 42ms - 20ms;
    REQUIRE(data == expected);
}
