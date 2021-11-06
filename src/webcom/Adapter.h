#pragma once

#include <any>

namespace webcom {

struct Service;

struct Adapter {
    using SendData = std::function<void(std::string_view)>;
    using GetSize  = std::function<size_t()>;

    SendData& sendData;
    GetSize&  getBufferedAmount;
    Service&  service;

    template <typename ...Args>
    void sendBack(std::string_view _actionName, Args&&... _args) const;

    template <typename ...Args>
    void sendAll(std::string_view _actionName, Args&&... _args) const;

    template <typename ...Args>
    void sendOthers(std::string_view _actionName, Args&&... _args) const;

    template <typename CB, typename ...Args>
    void send(std::string_view _actionName, CB const& _cb, Args&&... _args) const;

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
template <typename CB, typename ...Args>
void Adapter::send(std::string_view _actionName, CB const& _cb, Args&&... _args) const {
    auto msg = convertToMessage(service.name, _actionName, std::forward<Args>(_args)...);
    for (auto& [adapter, value] : service.objects) {
        cb(*adapter, msg);
    }
}

}
