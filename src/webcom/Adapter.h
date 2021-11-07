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

    struct Call {
        enum class Type {All, Back, Others};
        Type type;
        Adapter const& adapter;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const;
    };

    auto callAll(std::string_view _actionName) const {
        return Call{Call::Type::All, *this, _actionName};
    }
    auto callBack(std::string_view _actionName) const {
        return Call{Call::Type::Back, *this, _actionName};
    }
    auto callOthers(std::string_view _actionName) const {
        return Call{Call::Type::Others, *this, _actionName};
    }

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
void Adapter::Call::operator()(Args&&... _args) const {
    auto msg = convertToMessage(adapter.service.name, actionName, std::forward<Args>(_args)...);
    if (type == Type::All) {
        for (auto& [_adapter, value] : adapter.service.objects) {
            _adapter->sendData(msg);
        }
    } else if (type == Type::Back) {
        adapter.sendData(msg);
    } else if (type == Type::Others) {
        for (auto& [_adapter, value] : adapter.service.objects) {
            if (_adapter != &adapter) {
                _adapter->sendData(msg);
            }
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
