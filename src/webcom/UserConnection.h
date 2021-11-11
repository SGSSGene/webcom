#pragma once

#include "ViewController.h"

namespace webcom {

/**
 * Represents a connection to a single client (browser)
 *
 * \param sendData is a function to send data
 * \param viewControllers a list of ViewControllers that this connection currently has
 */
struct UserConnection {
    std::function<void(std::string_view)>                          sendData;
    std::map<std::string, std::unique_ptr<webcom::ViewController>> viewControllers;
};

}
