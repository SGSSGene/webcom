#pragma once

#include <any>

namespace webcom {

struct Service;

struct ViewController;

/*struct ViewControllerBase {
    thread_local static inline ViewController* gViewController;
    ViewController& viewController{*gViewController};

    virtual ~ViewControllerBase() = default;
};*/

struct ViewController {
    using SendData = std::function<void(std::string_view)>;
    using GetSize  = std::function<size_t()>;

    ViewController& viewController{*this};


    thread_local static inline SendData gSendData;
    thread_local static inline Service* gService{};
    SendData sendData{std::move(gSendData)};
    Service& service{*gService};

    ViewController() {

    }
    ViewController(ViewController const&) = delete;
    ViewController(ViewController&&) = delete;

/*    ViewController(SendData _sendData, Service& _service)
        : sendData{std::move(_sendData)}
        , service{_service}
    {}*/
    virtual ~ViewController();

    auto operator=(ViewController const&) -> ViewController = delete;
    auto operator=(ViewController&&) -> ViewController = delete;


    struct Call {
        enum class Type {All, Back, Others};
        Type type;
        ViewController const& viewController;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const;
    };

    /**
     * Call funciton _actionName on all remote peers
     */
    auto callAll(std::string_view _actionName) const {
        return Call{Call::Type::All, *this, _actionName};
    }

    /**
     * Call function _actionName on the client associated with this ViewController
     */
    auto callBack(std::string_view _actionName) const {
        return Call{Call::Type::Back, *this, _actionName};
    }

    /**
     * Call function _actionName on all remote peers, but excluding the client
     * associated with this ViewController
     */
    auto callOthers(std::string_view _actionName) const {
        return Call{Call::Type::Others, *this, _actionName};
    }

    template <typename T, typename ...Args>
    static auto make(Args&&... args) {
//        ViewControllerBase::gViewController = this;
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    void dispatchSignalFromClient(std::string_view _name, YAML::Node _node = {});
};
}
#include "Service.h"

namespace webcom {

ViewController::~ViewController() {
    service.removeViewController(*this);
}

void ViewController::dispatchSignalFromClient(std::string_view _name, YAML::Node _node) {
    service.dispatchSignalFromClient(_name, *this, _node);
}


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

}
