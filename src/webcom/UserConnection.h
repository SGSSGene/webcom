#pragma once

#include "ViewController.h"

namespace webcom {

/**
 * Represents a connection to a single client (browser)
 *
 * \param sendData is a function to send data
 * \param getBufferedAmount reports how much data is currently queued for sending
 * \param viewControllers a list of ViewControllers that this connection currently has
 */
struct UserConnection {
    std::function<void(std::string_view)>         sendData;
    std::function<size_t()>                       getBufferedAmount;
    std::map<std::string, std::unique_ptr<webcom::ViewController>> viewControllers;
};

}
