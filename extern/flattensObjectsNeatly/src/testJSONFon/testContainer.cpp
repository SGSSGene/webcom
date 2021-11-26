#include <catch2/catch.hpp>
#include <fon/json.h>
#include <fon/std/all.h>


TEST_CASE("test json serialization of std::string", "[json][std][string][serialize]") {
    auto data = std::string{"hallo welt"};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isString());
    REQUIRE(node.asString() == "hallo welt");
}
TEST_CASE("test json deserialization of std::string", "[json][std][string][deserialize]") {
    Json::Value node;
    node = "hallo welt";
    auto data = fon::json::deserialize<std::string>(node);
    REQUIRE(data == "hallo welt");
}

TEST_CASE("test json serialization of std::vector", "[json][std][vector][serialize]") {
    auto data = std::vector<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}

TEST_CASE("test json deserialization of std::vector", "[json][std][vector][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::vector<int32_t>>(node);
    REQUIRE(data == (std::vector<int32_t>{10, 20, 30}));
}
TEST_CASE("test json serialization of std::vector with strings", "[json][std][vector][string][serialize]") {
    auto data = std::vector<std::string>{"hallo", "welt", "!"};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asString() == "hallo");
    REQUIRE(node[1].asString() == "welt");
    REQUIRE(node[2].asString() == "!");
}

TEST_CASE("test json deserialization of std::vector with strings", "[json][std][vector][deserialize]") {
    Json::Value node;
    node[0] = "hallo";
    node[1] = "welt";
    node[2] = "!";
    auto data = fon::json::deserialize<std::vector<std::string>>(node);
    REQUIRE(data == (std::vector<std::string>{"hallo", "welt", "!"}));
}

TEST_CASE("test json serialization of std::array", "[json][std][array][serialize]") {
    auto data = std::array<int32_t, 3>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}

TEST_CASE("test json deserialization of std::array", "[json][std][array][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::array<int32_t, 3>>(node);
    REQUIRE(data == (std::array<int32_t, 3>{10, 20, 30}));
}

TEST_CASE("test json serialization of std::list", "[json][std][list][serialize]") {
    auto data = std::list<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}
TEST_CASE("test json deserialization of std::list", "[json][std][list][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::list<int32_t>>(node);
    REQUIRE(data == (std::list<int32_t>{10, 20, 30}));
}
TEST_CASE("test json serialization of std::forward_list", "[json][std][forward_list][serialize]") {
    auto data = std::forward_list<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}
TEST_CASE("test json deserialization of std::forward_list", "[json][std][forward_list][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::forward_list<int32_t>>(node);
    REQUIRE(data == (std::forward_list<int32_t>{10, 20, 30}));
}
TEST_CASE("test json serialization of std::deque", "[json][std][deque][serialize]") {
    auto data = std::deque<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}
TEST_CASE("test json deserialization of std::deque", "[json][std][deque][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::deque<int32_t>>(node);
    REQUIRE(data == (std::deque<int32_t>{10, 20, 30}));
}

TEST_CASE("test json serialization of std::set", "[json][std][set][serialize]") {
    auto data = std::set<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE(node[0].asInt() == 10);
    REQUIRE(node[1].asInt() == 20);
    REQUIRE(node[2].asInt() == 30);
}
TEST_CASE("test json deserialization of std::set", "[json][std][set][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::set<int32_t>>(node);
    REQUIRE(data == (std::set<int32_t>{10, 20, 30}));
}
TEST_CASE("test json serialization of std::unordered_set", "[json][std][unordered_set][serialize]") {
    auto data = std::unordered_set<int32_t>{10, 20, 30};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 3);
    REQUIRE((std::set<int32_t>{node[0].asInt(), node[1].asInt(), node[2].asInt()}) == (std::set<int32_t>{10, 20, 30}));
}
TEST_CASE("test json deserialization of std::unordered_set", "[json][std][unordered_set][deserialize]") {
    Json::Value node;
    node[0] = 10;
    node[1] = 20;
    node[2] = 30;
    auto data = fon::json::deserialize<std::unordered_set<int32_t>>(node);
    REQUIRE(data.size() == 3);
    REQUIRE(data.count(10) == 1);
    REQUIRE(data.count(20) == 1);
    REQUIRE(data.count(30) == 1);
}

TEST_CASE("test json serialization of std::map", "[json][std][map][serialize]") {
    auto data = std::map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 3);
    REQUIRE(node["k1"].asInt() == 10);
    REQUIRE(node["k2"].asInt() == 20);
    REQUIRE(node["k3"].asInt() == 30);
}
TEST_CASE("test json deserialization of std::map", "[json][std][map][deserialize]") {
    Json::Value node;
    node["k1"] = 10;
    node["k2"] = 20;
    node["k3"] = 30;
    auto data = fon::json::deserialize<std::map<std::string, int32_t>>(node);
    REQUIRE(data == (std::map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}}));
}

TEST_CASE("test json serialization of std::unordered_map", "[json][std][unordered_map][serialize]") {
    auto data = std::unordered_map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 3);
    REQUIRE(node["k1"].asInt() == 10);
    REQUIRE(node["k2"].asInt() == 20);
    REQUIRE(node["k3"].asInt() == 30);
}
TEST_CASE("test json deserialization of std::unordered_map", "[json][std][unordered_map][deserialize]") {
    Json::Value node;
    node["k1"] = 10;
    node["k2"] = 20;
    node["k3"] = 30;
    auto data = fon::json::deserialize<std::unordered_map<std::string, int32_t>>(node);
    REQUIRE(data == (std::unordered_map<std::string, int32_t>{{"k1", 10}, {"k2", 20}, {"k3", 30}}));
}
TEST_CASE("test json serialization of std::pair", "[json][std][pair][serialize]") {
    auto data = std::pair<std::string, int32_t>{"hallo welt", 42};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 2);
    REQUIRE(node["0"].asString() == "hallo welt");
    REQUIRE(node["1"].asInt() == 42);
}
TEST_CASE("test json deserialization of std::pair", "[json][std][pair][deserialize]") {
    Json::Value node;
    node["0"] = "hallo welt";
    node["1"] = 42;
    auto data = fon::json::deserialize<std::pair<std::string, int32_t>>(node);
    REQUIRE(data.first == "hallo welt");
    REQUIRE(data.second == 42);
}
TEST_CASE("test json serialization of std::tuple<>", "[json][std][tuple][serialize]") {
    auto data = std::tuple<>{};
    auto node = fon::json::serialize(data);
    REQUIRE(node.size() == 0);
}
TEST_CASE("test json deserialization of std::tuple<>", "[json][std][tuple][deserialize]") {
    Json::Value node;
    auto data = fon::json::deserialize<std::tuple<>>(node);
    REQUIRE(data == std::tuple<>{});
}

TEST_CASE("test json serialization of std::tuple<X>", "[json][std][tuple][serialize]") {
    auto data = std::tuple<std::string> {"hallo welt"};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 1);
    REQUIRE(node["0"].asString() == "hallo welt");
}
TEST_CASE("test json deserialization of std::tuple<X>", "[json][std][tuple][deserialize]") {
    Json::Value node;
    node["0"] = "hallo welt";
    auto data = fon::json::deserialize<std::tuple<std::string>>(node);
    REQUIRE(std::get<0>(data) == "hallo welt");
}


TEST_CASE("test json serialization of std::tuple<X, Y>", "[json][std][tuple][serialize]") {
    auto data = std::tuple<std::string, int32_t>{"hallo welt", 42};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isObject());
    REQUIRE(node.size() == 2);
    REQUIRE(node["0"].asString() == "hallo welt");
    REQUIRE(node["1"].asInt() == 42);
}
TEST_CASE("test json deserialization of std::tuple<X, Y>", "[json][std][tuple][deserialize]") {
    Json::Value node;
    node["0"] = "hallo welt";
    node["1"] = 42;
    auto data = fon::json::deserialize<std::tuple<std::string, int32_t>>(node);
    REQUIRE(std::get<0>(data) == "hallo welt");
    REQUIRE(std::get<1>(data) == 42);
}
TEST_CASE("test json serialization of std::optional", "[json][std][optional][serialize]") {
    auto data = std::optional<std::string>{"hallo welt"};
    auto node = fon::json::serialize(data);
    REQUIRE(node.isArray());
    REQUIRE(node.size() == 1);
    REQUIRE(node[0].isString());
    REQUIRE(node[0].asString() == "hallo welt");
}

TEST_CASE("test json deserialization of std::optional", "[json][std][optional][deserialize]") {
    Json::Value node;
    node[0] = "hallo welt";
    auto data = fon::json::deserialize<std::optional<std::string>>(node);
    REQUIRE(data.has_value());
    REQUIRE(data.value() == "hallo welt");
}

TEST_CASE("test json serialization of empty std::optional", "[json][std][optional][serialize]") {
    auto data = std::optional<std::string>{};
    auto node = fon::json::serialize(data);
    REQUIRE(node.size() == 0);
}
TEST_CASE("test json deserialization of empty std::optional", "[json][std][optional][deserialize]") {
    Json::Value node;
    auto data = fon::json::deserialize<std::optional<std::string>>(node);
    REQUIRE(not data.has_value());
}

TEST_CASE("test json serialization of std::variant (index 0)", "[json][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{std::string{"hallo welt"}};
    auto node = fon::json::serialize(data);

    REQUIRE(node.isObject());
    REQUIRE(node.size() == 2);

    REQUIRE(node["0"].isUInt());
    REQUIRE(node["0"].asUInt() == 0);
    REQUIRE(node["1"].asString() == "hallo welt");
}
TEST_CASE("test json deserialization of std::variant (index 0)", "[json][std][variant][deserialize]") {
    Json::Value node {Json::objectValue};
    node["0"] = 0;
    node["1"] = "hallo welt";
    auto data = fon::json::deserialize<std::variant<std::string, int32_t, bool>>(node);
    REQUIRE(data.index() == 0);
    REQUIRE(std::get<0>(data) == "hallo welt");
}

TEST_CASE("test json serialization of std::variant (index 1)", "[json][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{int32_t{42}};
    auto node = fon::json::serialize(data);

    REQUIRE(node.isObject());
    REQUIRE(node.size() == 2);

    REQUIRE(node["0"].asUInt() == 1);
    REQUIRE(node["1"].asInt() == 42);
}
TEST_CASE("test json deserialization of std::variant (index 1)", "[json][std][variant][deserialize]") {
    Json::Value node {Json::objectValue};
    node["0"] = 1;
    node["1"] = 42;
    auto data = fon::json::deserialize<std::variant<std::string, int32_t, bool>>(node);
    REQUIRE(data.index() == 1);
    REQUIRE(std::get<1>(data) == 42);
}

TEST_CASE("test json serialization of std::variant (index 2)", "[json][std][variant][serialize]") {
    auto data = std::variant<std::string, int32_t, bool>{true};
    auto node = fon::json::serialize(data);

    REQUIRE(node.isObject());
    REQUIRE(node.size() == 2);

    REQUIRE(node["0"].asUInt() == 2);
    REQUIRE(node["1"].asBool() == true);
}
TEST_CASE("test json deserialization of std::variant (index 2)", "[json][std][variant][deserialize]") {
    Json::Value node {Json::objectValue};
    node["0"] = 2;
    node["1"] = true;
    auto data = fon::json::deserialize<std::variant<std::string, int32_t, bool>>(node);
    REQUIRE(data.index() == 2);
    REQUIRE(std::get<2>(data) == true);
}

TEST_CASE("test json serialization of std::filesystem::path", "[json][std][filesystem][path][serialize]") {
    auto data = std::filesystem::path{"./myfile.txt"};
    auto node = fon::json::serialize(data);

    REQUIRE(node.isString());
    REQUIRE(node.asString() == "./myfile.txt");
}

TEST_CASE("test json deserialization of std::filesystem::path", "[json][std][filesystem][path][deserialize]") {
    Json::Value node;
    node = "./some_file";
    auto data = fon::json::deserialize<std::filesystem::path>(node);

    REQUIRE(data == "./some_file");
}

TEST_CASE("test json serialization of std::set<std::filesystem::path>", "[json][std][filesystem][set][path][serialize]") {
    auto data = std::set<std::filesystem::path>{{"./myfile.txt"}};

    auto node = fon::json::serialize(data);

    REQUIRE(node.isArray());
    REQUIRE(node[0].isString());
    REQUIRE(node[0].asString() == "./myfile.txt");
}

TEST_CASE("test json deserialization of std::set<std::filesystem::path>", "[json][std][filesystem][set][path][deserialize]") {
    Json::Value node;
    node[0] = "./some_file";
    auto data = fon::json::deserialize<std::set<std::filesystem::path>>(node);

    REQUIRE(data.size() == 1);
    REQUIRE(*data.begin() == "./some_file");
}

TEST_CASE("test json serialization of std::chrono::time_point<ms>", "[json][std][chrono][time_point][serialize]") {
    auto data = std::chrono::time_point<std::chrono::milliseconds>(std::chrono::milliseconds{42});
    auto node = fon::json::serialize(data);

    REQUIRE(node.isInt());
    REQUIRE(node.asInt() == 42);
}

TEST_CASE("test json deserialization of std::chrono::time_point<ms>", "[json][std][chrono][time_point][deserialize]") {
    Json::Value node;
    node = 42;
    auto data = fon::json::deserialize<std::chrono::time_point<std::chrono::milliseconds>>(node);
    REQUIRE(data == std::chrono::time_point<std::chrono::milliseconds>(std::chrono::milliseconds{42}));
}

TEST_CASE("test json serialization of std::chrono::time_point<ns>", "[json][std][chrono][time_point][serialize]") {
    auto data = std::chrono::time_point<std::chrono::nanoseconds>(std::chrono::nanoseconds{42});
    auto node = fon::json::serialize(data);

    REQUIRE(node.isInt());
    REQUIRE(node.asInt() == 42);
}

TEST_CASE("test json deserialization of std::chrono::time_point<ns>", "[json][std][chrono][time_point][deserialize]") {
    Json::Value node;
    node = 42;
    auto data = fon::json::deserialize<std::chrono::time_point<std::chrono::nanoseconds>>(node);
    REQUIRE(data == std::chrono::time_point<std::chrono::nanoseconds>(std::chrono::nanoseconds{42}));
}

TEST_CASE("test json serialization of std::filesystem::file_time_type", "[json][std][chrono][time_point][serialize]") {
    auto data = std::filesystem::file_time_type(std::chrono::nanoseconds{42});
    auto node = fon::json::serialize(data);

    REQUIRE(node.isInt());
    REQUIRE(node.asInt() == 42);
}

TEST_CASE("test json deserialization of std::filesystem::file_time_type", "[json][std][chrono][time_point][deserialize]") {
    Json::Value node;
    node = 42;
    auto data = fon::json::deserialize<std::filesystem::file_time_type>(node);
    REQUIRE(data == std::filesystem::file_time_type(std::chrono::nanoseconds{42}));
}

TEST_CASE("test json serialization of std::chrono::duration", "[json][std][chrono][duration][serialize]") {
    using namespace std::chrono_literals;
    auto data = 42ms - 20ms;
    auto node = fon::json::serialize(data);

    REQUIRE(node.isInt());
    REQUIRE(node.asInt() == 22);
}

TEST_CASE("test json deserialization of std::chrono::duration", "[json][std][chrono][duration][deserialize]") {
    using namespace std::chrono_literals;

    Json::Value node;
    node = 22;
    using duration = decltype(42ms - 20ms);
    auto data = fon::json::deserialize<duration>(node);
    REQUIRE(data == (42ms - 20ms));
}
