#pragma once

#include "../GuardedType.h"
#include "../View.h"

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
using ReadAndWriteValue = webcom::GuardedType<T>;

template <typename T>
struct ReadAndWriteValueView : webcom::View<ReadAndWriteValueView<T>> {
    ReadAndWriteValue<T>& entity;

    ReadAndWriteValueView(ReadAndWriteValue<T>& _entity)
        : entity{_entity}
    {
        auto&& [g, value] = *entity;
        // call 'init' of this client only
        this->callBack("init")(value);
    }

    static void reflect(auto& visitor) {
        visitor("setValue", &ReadAndWriteValueView<T>::setValue);
    }

    void setValue(T t) {
        auto&& [g, value] = *entity;
        value = std::move(t);
        // call 'init' of this client only
        this->callAll("setValue")(value);

    }
};
}
