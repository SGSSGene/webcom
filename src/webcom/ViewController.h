#pragma once

#include <any>

namespace webcom {

struct Service;

struct ViewController {
    using SendData = std::function<void(YAML::Node)>;
    using GetSize  = std::function<size_t()>;

    thread_local static inline SendData gSendData;
    thread_local static inline Service* gService{};
    SendData sendData{std::move(gSendData)};
    Service& service{*gService};

    ViewController() = default;
    ViewController(ViewController const&) = delete;
    ViewController(ViewController&&) = delete;

    virtual ~ViewController() {
        _dtor();
    }
    template <typename X = void>
    void _dtor() {
        service.removeViewController(*this);
    }


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

    void dispatchSignalFromClient(YAML::Node _node) {
        _dispatchSignalFromClient(_node);
    }

    template <typename X = void>
    void _dispatchSignalFromClient(YAML::Node _node) {
        service.dispatchSignalFromClient(*this, _node);
    }
};

template <typename T, typename ...Args>
static auto make(Args&&... args) {
    return std::make_unique<T>(std::forward<Args>(args)...);
}

namespace detail {
template <typename ...Args>
auto to_yaml(Args&&...args) -> YAML::Node {
    return fon::yaml::serialize<std::tuple<std::decay_t<Args>...>>(std::tuple{std::forward<Args>(args)...});
}

template <typename ...Args>
auto convertToMessage(std::string_view _actionName, Args&&... _args) -> YAML::Node {
    auto node = YAML::Node{};
    node["action"]  = std::string{_actionName};
    node["params"]  = to_yaml(std::forward<Args>(_args)...);

    return node;
}
}

template <typename ...Args>
void ViewController::Call::operator()(Args&&... _args) const {
    auto msg = detail::convertToMessage(actionName, std::forward<Args>(_args)...);
    if (type == Type::All) {
        for (auto& _viewController : viewController.service.getViewControllers()) {
            _viewController->sendData(msg);
        }
    } else if (type == Type::Back) {
        viewController.sendData(msg);
    } else if (type == Type::Others) {
        for (auto& _viewController : viewController.service.getViewControllers()) {
            if (_viewController != &viewController) {
                _viewController->sendData(msg);
            }
        }
    }
}

}
