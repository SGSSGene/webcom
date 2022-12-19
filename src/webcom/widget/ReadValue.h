#pragma once

#include "../GuardedType.h"
#include "../View.h"

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
struct ReadValue : webcom::GuardedType<T> {
    struct View : webcom::View<View> {
        ReadValue<T>& entity;

        View(ReadValue<T>& _entity)
            : entity{_entity}
        {
            auto&& [g, value] = *entity;
            // call 'init' of this client only
            this->callBack("init")(value);
        }

        static constexpr void reflect(auto& visitor) {}
    };
};
}
