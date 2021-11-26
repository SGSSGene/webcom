#include "Server.h"

#include <webcom/widget/all.h>

#include <fmt/color.h>
#include <fmt/format.h>

#include <thread>
#include <chrono>

/**
 * A very simple chat....it is just a list with a lock
 **/
using Chat = webcom::GuardedType<std::vector<std::string>>;

/** This represents the View (MVC) of a single User accessing the chat
 *
 * Each user (connection via websocket) will have its own view
 */
struct ChatView : webcom::View<ChatView> {
    Chat& chat;

    ChatView(Chat& _chat)
        : chat{_chat}
    {
        auto&& [g, list] = *chat;
        // call 'init' of only this client
        callBack("init")(list);
    }

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("addText", &ChatView::addText);
    }

    void addText(std::string str) {
        chat->push_back(str);
        // call 'addMsg' of all clients
        callAll("addMsg")(str);
    }
};

int main(int argc, char const* const* argv) {
    Server server;

    // Some magic container providing the web server
    auto cndlServices = webcom::CndlServices{server.cndlServer, "/ws"};

    Chat chat;
    auto chatController = webcom::Controller<ChatView>{};
    cndlServices.addController("chat", [&](webcom::Services::SendCB _send) {
        // create access, in theory we could do an access check here
        return chatController.makeView(std::move(_send), chat);
    });

    auto readValue = webcom::widget::ReadValue<size_t>{};
    auto readValueController = webcom::Controller<webcom::widget::ReadValueView<size_t>>{};
    cndlServices.addController("readValue", [&](webcom::Services::SendCB _send) {
        return readValueController.makeView(std::move(_send), readValue);
    });

    auto readAndWriteValue = webcom::widget::ReadAndWriteValue<size_t>{};
    auto readAndWriteValueController = webcom::Controller<webcom::widget::ReadAndWriteValueView<size_t>>{};
    cndlServices.addController("readAndWriteValue", [&](webcom::Services::SendCB _send) {
        return readAndWriteValueController.makeView(std::move(_send), readAndWriteValue);
    });

    auto t = std::thread{[&]() {
        size_t x = {0};
        while(true) {
            std::this_thread::sleep_for(std::chrono::milliseconds{100});
            auto&& [g, value] = *readValue;
            value = ++x;
            readValueController.callAll("setValue")(value);
        }
    }};


    server.run();
}
