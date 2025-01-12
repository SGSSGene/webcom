// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "Controller.h"

namespace webcom {

struct View {
    using SendData = std::function<void(Json::Value)>;
    thread_local static inline SendData gSendData;
    SendData sendData{std::move(gSendData)};

    std::function<void(Json::Value)> dispatchSignalFromClient;

    std::function<void()> cleanup;
    std::function<channel::value_mutex<std::unordered_set<View*>> const&()> getViews;

    View() = default;
    View(View const&) = delete;
    View(View&&) = delete;

    virtual ~View() {
        cleanup();
    }

    auto operator=(View const&) -> View = delete;
    auto operator=(View&&) -> View = delete;

    struct Call {
        enum class Type {All, Back, Others};
        Type type;
        View const& view;
        std::string_view methodName;

        template <typename ...Args>
        void operator()(Args&&... _args) const {
            auto msg = detail::convertToMessage(methodName, std::forward<Args>(_args)...);
            if (type == Type::All) {
                auto [guard, views] = *view.getViews();
                for (auto& _view : *views) {
                    _view->sendData(msg);
                }
            } else if (type == Type::Back) {
                view.sendData(msg);
            } else if (type == Type::Others) {
                auto [guard, views] = *view.getViews();
                for (auto& _view : *views) {
                    if (_view != &view) {
                        _view->sendData(msg);
                    }
                }
            }
        }
    };

    /**
     * Call function _methodName on all remote peers
     */
    auto callAll(std::string_view _methodName) const {
        return Call{Call::Type::All, *this, _methodName};
    }

    /**
     * Call function _methodName on the client associated with this View
     */
    auto callBack(std::string_view _methodName) const {
        return Call{Call::Type::Back, *this, _methodName};
    }

    /**
     * Call function _methodName on all remote peers, but excluding the client
     * associated with this View
     */
    auto callOthers(std::string_view _methodName) const {
        return Call{Call::Type::Others, *this, _methodName};
    }
};

}
