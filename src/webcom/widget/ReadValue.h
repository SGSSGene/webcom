// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: AGPL-3.0-or-later
#pragma once

#include "../View.h"

#include <channel/value_mutex.h>

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
struct ReadValue : channel::value_mutex<T> {
    struct View : webcom::View {
        ReadValue<T>& entity;

        View(ReadValue<T>& _entity)
            : entity{_entity}
        {
            auto [g, value] = *entity;
            // call 'init' of this client only
            callBack("init", *value);
        }

        void setValue(T v) {
            auto [g, value] = *entity;
            *value = v;

            callAll("setValue", v);
        }
    };
};
}
