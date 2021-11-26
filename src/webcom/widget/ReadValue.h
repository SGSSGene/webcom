#pragma once

#include "../GuardedType.h"
#include "../View.h"

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
using ReadValue = webcom::GuardedType<T>;

template <typename T>
struct ReadValueView : webcom::View<ReadValueView> {
    ReadValue<T>& entity;

    ReadValueView(ReadValue<T>& _entity)
        : entity{_entity}
    {
        auto&& [g, value] = *entity;
        // call 'init' of this client only
        callBack("init")(value);
    }

    static constexpr void reflect(auto& visitor) {}
};
}
