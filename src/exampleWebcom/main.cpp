#include <fon/yaml.h>
#include <fon/std/all.h>
#include <webcom/webcom.h>

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

    // Some magic container providing the web server
    auto webServices = webcom::WebServices{"../share/exampleWebcom/index.html", {{"/webcom/(.*)", "../share/webcom"}, {"/(.*)", "../share/exampleWebcom"}}};

    Chat chat;
    webServices.provideViewController("chat", [&]() {
        // create access, in theory we could do an access check here
        return webcom::make<ChatViewController>(chat);
    });

    webServices.listen(simplyfile::getHosts("127.0.0.1", std::to_string(8080))).getEpoll();
    while(true) {
        webServices.epoll.work();
    }




}
