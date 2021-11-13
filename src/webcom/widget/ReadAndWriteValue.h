#pragma once

namespace webcom::widget {

// A value that can only be read by the front end
template <typename T>
using ReadAndWriteValue = webcom::GuardedType<T>;

template <typename T>
struct ReadAndWriteValueViewController : webcom::ViewController {
    ReadAndWriteValue<T>& entity;

    ReadAndWriteValueViewController(ReadAndWriteValue<T>& _entity)
        : entity{_entity}
    {
        auto&& [g, value] = *entity;
        // call 'init' of this client only
        callBack("init")(value);
    }

    static void reflect(auto& visitor) {
        visitor("setValue", &ReadAndWriteValueViewController<T>::setValue);
    }

    void setValue(T t) {
        auto&& [g, value] = *entity;
        value = std::move(t);
        // call 'init' of this client only
        callAll("setValue")(value);

    }
};
}
