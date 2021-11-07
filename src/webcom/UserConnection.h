#pragma once

#include "ViewController.h"

namespace webcom {

struct UserConnection {
    std::function<void(std::string_view)>         sendData;
    std::function<size_t()>                       getBufferedAmount;
    std::map<std::string, webcom::ViewController> viewControllers;
};

}
