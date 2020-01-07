#pragma once

#if 0

#include "asFunction.h"

#include <flattensObjectsNeatly/flattensObjectsNeatly.h>
#include <slots/slots.h>
#include <any>

namespace webcom {

template <typename T>
struct Viewer;

enum class Lossless {
	True,
	False
};


struct ViewerBase {
	using Sender = std::function<void(YAML::Node, Lossless)>;

	std::vector<onExit::OnExit> mHandles;
	std::map<std::string, std::function<void(YAML::Node)>, std::less<>> mDispatcher;

	Sender sender;

	ViewerBase(Sender _sender)
		: sender {_sender}
	{}

	template <typename T, typename CB>
	void connectModel(slots::Slot<T>& slot, CB cb) {
		mHandles.emplace_back(slot.connectSignalHandler(cb));
	}

	template <typename ...Args>
	void connectClient(std::string const& str, std::function<void(Args...)> cb) {
		mDispatcher[str] = [cb](YAML::Node node) {
			auto argsAsTuple = fon::yaml::deserialize<std::tuple<Args...>>(node);

			std::apply([&cb](auto... args) {
				cb(args...);
			}, argsAsTuple);
		};
	}
	template <typename L>
	void connectClient(std::string const& str, L l) {
		connectClient(str, asFunction<L>{l});
	}

	template <typename Arg>
	void sendToClient(Lossless lossless, std::string const& _str, Arg&& arg) {
		YAML::Node msg;
		msg["action"] = _str;
		msg["params"] = fon::yaml::serialize(std::forward<Arg>(arg));
		sender(msg, lossless);
	}


	void dispatchMessage(YAML::Node node) {
		if (not node.IsMap()) {
			throw std::runtime_error("not correct format");
		}
		mDispatcher.at(node["action"].as<std::string>())(node["params"]);
	}
};

}
#endif
