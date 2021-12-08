#pragma once

#include "Controller.h"

#include <any>

namespace webcom {

struct ViewBase {
    using SendData = std::function<void(Json::Value)>;

    thread_local static inline SendData       gSendData;
    SendData sendData{std::move(gSendData)};

    virtual ~ViewBase() = default;

    virtual void dispatchSignalFromClient(Json::Value _node) = 0;

};
template <typename T>
struct View : ViewBase {
    thread_local static inline Controller<T>* gController{};
    Controller<T>& controller{*gController};

    View() = default;
    View(View const&) = delete;
    View(View&&) = delete;

    virtual ~View() {
        controller.removeView(*this);
    }

    auto operator=(View const&) -> View = delete;
    auto operator=(View&&) -> View = delete;

    struct Call {
        enum class Type {All, Back, Others};
        Type type;
        View const& view;
        std::string_view actionName;

        template <typename ...Args>
        void operator()(Args&&... _args) const {
            auto msg = detail::convertToMessage(actionName, std::forward<Args>(_args)...);
            if (type == Type::All) {
                auto&& [guard, views] = *view.controller.getViews();
                for (auto& _view : views) {
                    _view->sendData(msg);
                }
            } else if (type == Type::Back) {
                view.sendData(msg);
            } else if (type == Type::Others) {
                auto&& [guard, views] = *view.controller.getViews();
                for (auto& _view : views) {
                    if (_view != &view) {
                        _view->sendData(msg);
                    }
                }
            }
        }
    };

    /**
     * Call funciton _actionName on all remote peers
     */
    auto callAll(std::string_view _actionName) const {
        return Call{Call::Type::All, *this, _actionName};
    }

    /**
     * Call function _actionName on the client associated with this View
     */
    auto callBack(std::string_view _actionName) const {
        return Call{Call::Type::Back, *this, _actionName};
    }

    /**
     * Call function _actionName on all remote peers, but excluding the client
     * associated with this View
     */
    auto callOthers(std::string_view _actionName) const {
        return Call{Call::Type::Others, *this, _actionName};
    }

    void dispatchSignalFromClient(Json::Value _node) override {
        controller.dispatchSignalFromClient(*this, _node);
    }

};
}
