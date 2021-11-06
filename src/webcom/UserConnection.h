#pragma once

#include "Adapter.h"

namespace webcom {

struct UserConnection {
    std::function<void(YAML::Node)>        sendData;
    std::map<std::string, webcom::Adapter> adapters;
};

}
