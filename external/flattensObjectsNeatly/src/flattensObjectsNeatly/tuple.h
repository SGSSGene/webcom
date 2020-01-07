#pragma once

#include "flattensObjectsNeatly.h"

#include <tuple>

namespace fon {

template <typename Node, typename ...Args>
struct convert<Node, std::tuple<Args...>> {

	template <int I=0, typename L>
	static void match(std::tuple<Args...>const &args, int i, L l) {
		if (I == i) {
			l(std::get<I>(args));
			return;
		}
		if constexpr (I < sizeof...(Args)) {
			match(args, i+1);
		}
	}

	static constexpr Type type = Type::List;
	struct Infos {
		using Key   = size_t;
		template <typename L, int I=0>
		static void range(std::tuple<Args...>& obj, L const& l) {
			if constexpr (I < sizeof...(Args)) {
				l(I, obj);
				range<L, I+1>(obj, l);
			}
		};

		static void reserve(std::tuple<Args...>&, size_t) {}

		template <typename N2>
		static auto emplace(N2& node, std::tuple<Args...>& obj, Key key) {
			if (key >= N or key < 0) {
				throw std::runtime_error("accessing array out of range");
			}
			match(obj, key, [&](auto& o) {
				node[key] % o;
			});
		}
	};

	convert(Node& node, std::tuple<Args...>& obj) {
		for (size_t i{0}; i < N; ++i) {
			match(obj, i, [&](auto& o) {
				node[key] % o;
			});
		}
	}
};

}
