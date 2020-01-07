#pragma once

#include <any>
#include <yaml-cpp/yaml.h>

namespace webcom {

struct Service;

struct Adapter {
	using CB = std::function<void(YAML::Node)>;
	CB&       sendData;
	Service&  service;

	template <typename ...Args>
	void sendBack(std::string_view _actionName, Args&&... _args) const;

	template <typename ...Args>
	void sendAll(std::string_view _actionName, Args&&... _args) const;

	template <typename ...Args>
	void sendOthers(std::string_view _actionName, Args&&... _args) const;

	template <typename T, typename ...Args>
	auto make(Args&&... args) {
		return std::make_shared<T>(std::forward<Args>(args)...);
	}

	operator bool() const {
		return service;
	}
};
}
#include "Service.h"

namespace webcom {

template <typename ...Args>
void Adapter::sendBack(std::string_view _actionName, Args&&... _args) const {
	auto msg = convertToMessage(service.name, _actionName, std::forward<Args>(_args)...);
	sendData(msg);
}
template <typename ...Args>
void Adapter::sendAll(std::string_view _actionName, Args&&... _args) const {
	auto msg = convertToMessage(service.name, _actionName, std::forward<Args>(_args)...);
	for (auto& [adapter, value] : service.objects) {
		adapter->sendData(msg);
	}
}
template <typename ...Args>
void Adapter::sendOthers(std::string_view _actionName, Args&&... _args) const {
	auto msg = convertToMessage(service.name, _actionName, std::forward<Args>(_args)...);
	for (auto& [adapter, value] : service.objects) {
		if (adapter != this) {
			adapter->sendData(msg);
		}
	}
}


}
