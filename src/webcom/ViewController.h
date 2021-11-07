#pragma once

#include <any>

namespace webcom {

struct Service;

struct ViewController {
    using SendData = std::function<void(std::string_view)>;
    using GetSize  = std::function<size_t()>;

    SendData& sendData;
    GetSize&  getBufferedAmount;
    Service&  service;

    struct Call {
        enum class Type {All, Back, Others};
        Type type;
        ViewController const& viewController;
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

    template <typename T, typename ...Args>
    auto make(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

    operator bool() const;
};
}
#include "Service.h"

namespace webcom {

template <typename ...Args>
void ViewController::Call::operator()(Args&&... _args) const {
    auto msg = detail::convertToMessage(viewController.service.getName(), actionName, std::forward<Args>(_args)...);
    if (type == Type::All) {
        for (auto& [_viewController, value] : viewController.service.getViewControllers()) {
            _viewController->sendData(msg);
        }
    } else if (type == Type::Back) {
        viewController.sendData(msg);
    } else if (type == Type::Others) {
        for (auto& [_viewController, value] : viewController.service.getViewControllers()) {
            if (_viewController != &viewController) {
                _viewController->sendData(msg);
            }
        }
    }
}

ViewController::operator bool() const {
    return service;
}

}
