// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Controller.h"

namespace webcom {

struct ViewBase {
    using SendData = std::function<void(Json::Value)>;
    thread_local static inline SendData gSendData;
    SendData sendData{std::move(gSendData)};

    virtual ~ViewBase() = default;
protected:
    template <typename T>
    void detachFromController(Controller<T>& controller) {
        controller.removeView(*this);
    }
public:


    std::function<void(Json::Value)> dispatchSignalFromClient;

//    virtual void dispatchSignalFromClient(Json::Value _node) = 0;
};

template <typename TTT>
struct View : ViewBase {
    thread_local static inline Controller<TTT>* gController{};
    Controller<TTT>& controller{*gController};

    View() {
        dispatchSignalFromClient = [this](Json::Value msg) {
            auto& view = static_cast<TTT&>(*this);

            auto action = msg["action"].as<std::string>();

            // calls the correct function from ::reflect
            auto selector = detail::FunctionSelector{action, [&](auto func) {
                using Params = typename signature_t<std::decay_t<decltype(func)>>::params_t;
                auto paramsAsTuple = fon::json::deserialize<Params>(msg["params"]);
                std::apply([&](auto&&... params) {
                    (view.*func)(std::forward<decltype(params)>(params)...);
                }, paramsAsTuple);
            }};
            TTT::reflect(selector);
        };
    }
    View(View const&) = delete;
    View(View&&) = delete;

    virtual ~View() {
        ViewBase::detachFromController(controller);
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
                auto [guard, views] = *view.controller.getViews();
                for (auto& _view : *views) {
                    _view->sendData(msg);
                }
            } else if (type == Type::Back) {
                view.sendData(msg);
            } else if (type == Type::Others) {
                auto [guard, views] = *view.controller.getViews();
                for (auto& _view : *views) {
                    if (_view != &view) {
                        _view->sendData(msg);
                    }
                }
            }
        }
    };

    /**
     * Call function _actionName on all remote peers
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
};

}
