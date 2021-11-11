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
        viewController.callBack("init")(list);
    }

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("addText", &ChatViewController::addText);
    }

    void addText(std::string str) {
        auto&& [g, list] = *chat;
        list.push_back(str);
        // call 'addMsg' of all clients
        viewController.callAll("addMsg")(str);
    }
};

int main(int argc, char const* const* argv) {
    Chat chat;

    auto chatService = webcom::Service("chat", [&]() {
        // create access, in theory we could do an access check here
        return webcom::ViewController::make<ChatViewController>(chat);
    });

    auto vc = chatService.createViewController([](std::string_view data) {
        fmt::print("sending data:\n{}\n---\n\n", data);
    });

    {
        auto msg = YAML::Node{};
        msg["action"] = "addText";
        msg["params"].push_back("test");
        vc->dispatchSignalFromClient(msg);
    }

    {
        auto msg = YAML::Node{};
        msg["action"] = "addText";
        msg["params"].push_back("secnd entry");
        vc->dispatchSignalFromClient(msg);
    }
    {
       auto vc = chatService.createViewController([](std::string_view data) {
            fmt::print("second user:\n{}\n---\n\n", data);
        });

    }

//    vc->dispatchMessageFrom(msg);
}
