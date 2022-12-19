#pragma once

#include "../GuardedType.h"
#include "../View.h"

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
struct ReadAndWriteValue : webcom::GuardedType<T> {
    struct View : webcom::View<View> {
        ReadAndWriteValue<T>& entity;

        View(ReadAndWriteValue<T>& _entity)
            : entity{_entity}
        {
            auto&& [g, value] = *entity;
            // call 'init' of this client only
            this->callBack("init")(value);
        }

        static void reflect(auto& visitor) {
            visitor("setValue", &View::setValue);
        }

        void setValue(T t) {
            auto&& [g, value] = *entity;
            value = std::move(t);
            // call 'init' of this client only
            this->callAll("setValue")(value);

        }
    };
};
}
