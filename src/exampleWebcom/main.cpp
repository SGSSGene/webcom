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
    struct View : webcom::View<View> {
        Chat& chat;

        View(Chat& _chat)
            : chat{_chat}
        {
            auto [g, list] = *chat;
            // call 'init' of only this client
            callBack("init")(*list);
        }

        static constexpr void reflect(auto& visitor) {
            // function that can be called by the client (webbrowser)
            visitor("addText", &View::addText);
        }

        void addText(std::string str) {
            chat->push_back(str);
            // call 'addMsg' of all clients
            callAll("addMsg")(str);
        }
    };
};

int main(int argc, char const* const* argv) {
    Server server;

    // Some magic container providing the web server
    auto cndlServices = webcom::CndlServices{server.cndlServer, "/ws"};

    Chat chat;
    cndlServices.setController("chat", chat);

    auto readValue = webcom::widget::ReadValue<size_t>{};
    auto readValueController = cndlServices.setController("readValue", readValue);

    auto readAndWriteValue = webcom::widget::ReadAndWriteValue<size_t>{};
    cndlServices.setController("readAndWriteValue", readAndWriteValue);


    auto t = std::thread{[&]() {
        size_t x = {0};
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            auto&& [g, value] = *readValue;
            *value = ++x;
            readValueController->callAll("setValue")(*value);
        }
    }};


    server.run();
}
