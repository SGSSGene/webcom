#pragma once

#include "Adapter.h"

namespace webcom {

struct UserConnection {
    std::function<void(std::string_view)>  sendData;
    std::function<size_t()>                getBufferedAmount;
    std::map<std::string, webcom::Adapter> adapters;
};

}
