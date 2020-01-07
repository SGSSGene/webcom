#include <catch/catch.hpp>
#include <flattensObjectsNeatly/flattensObjectsNeatly.h>
//#include <flattensObjectsNeatly/tuple.h>

TEST_CASE("test yaml serialization of tuple", "[yaml][tuple]") {
	SECTION("single entry tuple") {
		auto data = std::tuple<int8_t, int8_t> {5, 6};
		auto node = fon::yaml::serialize(data);
		REQUIRE(node.IsSequence());
		REQUIRE(node[0].as<int8_t>() == 5);
		REQUIRE(node[1].as<int8_t>() == 6);
	}
}
