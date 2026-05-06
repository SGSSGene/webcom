// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: CC0-1.0
#include "Server.h"

#include <chrono>
#include <fmt/color.h>
#include <fmt/format.h>
#include <thread>
#include <webcom/widget/all.h>

/**
 * A very simple chat....it is just a list with a lock
 **/
struct Chat : channel::value_mutex<std::vector<std::string>> {

    /** This represents the View (MVC) of a single User accessing the chat
     *
     * Each user (connection via websocket) will have its own view
     */
    struct View : webcom::View {
        Chat& chat;

        View(Chat& _chat)
            : chat{_chat}
        {
            // function that can be called by the client (webbrowser)
            registerMethod("addText", &View::addText);

            auto [g, list] = *chat;
            // call 'init' of only this client
            callBack("init", *list);
        }

        void addText(std::string str) {
            chat->push_back(str);
            // call 'addMsg' of all clients
            callAll("addMsg", str);
        }
    };
};

int main(int argc, char const* const* argv) {
    Server server;

    // Some magic container providing the web server
    auto cndlServices = webcom::CndlServices{server.cndlServer, "/ws"};

    auto chatController              = webcom::Controller<Chat>{};
    auto readValueController         = webcom::Controller<webcom::widget::ReadValue<size_t>>{};
    auto readAndWriteValueController = webcom::Controller<webcom::widget::ReadAndWriteValue<size_t>>{};

    cndlServices.registerController("chat", chatController);
    cndlServices.registerController("readValue", readValueController);
    cndlServices.registerController("readAndWriteValue", readAndWriteValueController);

    auto t = std::thread{[&]() {
        size_t x = {0};

        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            readValueController->setValue(++x);
        }
    }};
    server.run();
    cndlServices.removeController("chat");
    cndlServices.removeController("readValue");
    cndlServices.removeController("readAndWriteValue");
}
