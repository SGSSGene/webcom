#include "Server.h"

#include <fmt/color.h>
#include <fmt/format.h>

/**
 * A very simple chat....it is just a list with a lock
 **/
using Chat = webcom::GuardedType<std::vector<std::string>>;

/** This represents the Controller and View (MVC) of a single User accessing the chat
 *
 * Each user (connection via websocket) will have its own view
 */
struct ChatViewController : webcom::ViewController {
    Chat& chat;

    ChatViewController(Chat& _chat)
        : chat{_chat}
    {
        auto&& [g, list] = *chat;
        // call 'init' of only this client
        callBack("init")(list);
    }

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("addText", &ChatViewController::addText);
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
    auto cndlServices = webcom::CndlServices<size_t>{server.cndlServer, "/ws"};

    Chat chat;
    cndlServices.provideViewController("chat", [&](size_t) {
        // create access, in theory we could do an access check here
        return webcom::make<ChatViewController>(chat);
    });

    server.run();
}
