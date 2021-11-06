#include <catch2/catch.hpp>
#include <fon/binary.h>
#include <fon/std/all.h>

namespace {
struct A {
    std::vector<int32_t> xs;
    int32_t* ptr{nullptr};

    constexpr static void reflect(auto& visitor, auto& self) {
        visitor["xs"]  % self.xs;
        visitor["ptr"] % self.ptr;
    }
};
}

TEST_CASE("test binary serialization of raw pointers", "[binary][raw][pointer][serialize]") {
    auto data = A{{10, 20, 30}};
    data.ptr = &data.xs[1];

    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "xs"
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},  std::byte{'s'},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "ptr"
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'x'},  std::byte{'s'},  std::byte{'/'},
        std::byte{'1'}
    };
    REQUIRE(buffer == expected);
}


TEST_CASE("test binary deserialization of raw pointers", "[binary][raw][pointer][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "xs"
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},  std::byte{'s'},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs.size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x0a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[0]
        std::byte{0x14}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[1]
        std::byte{0x1e}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // xs[2]
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // "ptr"
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'x'},  std::byte{'s'},  std::byte{'/'},
        std::byte{'1'}
    };

    auto data = fon::binary::deserialize<A>(input);

    REQUIRE(data.xs == (std::vector<int32_t>{10, 20, 30}));
    REQUIRE(data.ptr == &data.xs[1]);
}

TEST_CASE("test binary serialization of unique_ptr pointers", "[binary][std][unique_ptr][pointer][serialize]") {
    auto data = std::make_unique<int32_t>(42);
    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // 42
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of unique_ptr pointers", "[binary][std][unique_ptr][pointer][deserialize]") {
    auto input = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // size()
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // 42
    };

    auto data = fon::binary::deserialize<std::unique_ptr<int32_t>>(input);
    REQUIRE(data);
    CHECK(*data == 42);
}

namespace {
struct B {
    std::unique_ptr<int32_t> uptr;
    int* ptr{nullptr};

    static void reflect(auto& visitor,auto& self) {
        visitor["uptr"] % self.uptr;
        visitor["ptr"]  % self.ptr;
    }
};
}

TEST_CASE("test binary serialization of unique_ptr pointers and raw pointers", "[binary][std][unique_ptr][raw][pointer][serialize][struct]") {
    auto data = B{};
    data.uptr = std::make_unique<int32_t>(42);
    data.ptr = data.uptr.get();

    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte> {
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["uptr"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'u'},  std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.uptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2a}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["ptr"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'u'},  std::byte{'p'},  std::byte{'t'},
        std::byte{'r'},  std::byte{'/'},  std::byte{'0'},
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of unique_ptr pointers and raw pointers", "[binary][std][unique_ptr][raw][pointer][deserialize][struct]") {
    auto input = std::vector<std::byte>{
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["uptr"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'u'},  std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.uptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x2b}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x03}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["ptr"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'p'},  std::byte{'t'},  std::byte{'r'},
        std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'u'},  std::byte{'p'},  std::byte{'t'},
        std::byte{'r'},  std::byte{'/'},  std::byte{'0'},
    };

    auto data = fon::binary::deserialize<B>(input);
    REQUIRE(data.uptr);
    REQUIRE(*data.uptr == 43);
    REQUIRE(data.ptr == data.uptr.get());
}

namespace {
struct Base {
    int32_t x;

    static void reflect(auto& visitor, auto& self) {
        visitor["x"] % self.x;
    }
};
struct Derived : Base {
    int32_t y;
    Base* self;

    static void reflect(auto& visitor, auto& self) {
        Base::reflect(visitor, self);
        visitor["y"]    % self.y;
        visitor["self"] % self.self;
    }
};
}

TEST_CASE("test binary serialization of base pointers", "[binary][raw][base][pointer][serialize][struct]") {
    auto data = std::make_unique<Derived>();
    data->x = 5;
    data->y = 6;
    data->self = data.get();

    auto buffer = fon::binary::serialize(data);

    auto expected = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // unique_ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["x"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},
        std::byte{0x05}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.x
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["y"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'y'},
        std::byte{0x06}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.y
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["self"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'s'},  std::byte{'e'},  std::byte{'l'},  std::byte{'f'},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.self
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'0'},
    };

    REQUIRE(buffer == expected);
}

TEST_CASE("test binary deserialization of base pointers", "[binary][raw][base][pointer][deserialize][struct]") {
    auto input = std::vector<std::byte>{
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // unique_ptr
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["x"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'x'},
        std::byte{0x07}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.x
        std::byte{0x01}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["y"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'y'},
        std::byte{0x08}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.y
        std::byte{0x04}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B["self"]
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'s'},  std::byte{'e'},  std::byte{'l'},  std::byte{'f'},
        std::byte{0x02}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, // B.self
        std::byte{0x00}, std::byte{0x00}, std::byte{0x00}, std::byte{0x00},
        std::byte{'/'},  std::byte{'0'},
    };

    auto data = fon::binary::deserialize<std::unique_ptr<Derived>>(input);
    REQUIRE(data != nullptr);
    CHECK(data->x == 7);
    CHECK(data->y == 8);
    CHECK(data->self == data.get());
}
