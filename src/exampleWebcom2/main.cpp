#include <fon/yaml.h>
#include <fon/std/all.h>
#include <webcom/webcom.h>

#include <fmt/color.h>
#include <fmt/format.h>

/**
 * A very simple chat....it is just a list with a lock
 **/
using Chat = webcom::GuardedType<std::vector<std::string>>;

struct UserViewController : webcom::ViewController {
    webcom::Services& services;
    std::unordered_map<std::string, std::unique_ptr<webcom::ViewController>> viewControllers;

    UserViewController(webcom::Services& _services)
        : services{_services}
    {}

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("subscribe", &UserViewController::subscribe);
        visitor("unsubscribe", &UserViewController::unsubscribe);
        visitor("message", &UserViewController::message);
    }

    void subscribe(std::string _serviceName) {
        viewControllers.try_emplace(_serviceName, services.subscribe(_serviceName, [&](YAML::Node _node) {
            callBack("data")(_node);
        }));
    }

    void unsubscribe(std::string _serviceName) {
        viewControllers.erase(_serviceName);
    }

    void message(YAML::Node data) {
//        auto service = data["service"].as<std::string>();
//        viewControllers.at(service)->dispatchSignalFromClient(data["params"]);
    }
};



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
    auto services = webcom::Services{};

    Chat chat;

    auto& userService = services.provideViewController("services", [&]() {
        // create access, in theory we could do an access check here
        return webcom::make<UserViewController>(services);
    });
/*    auto uv = userService.createViewController([](std::string_view data) {
    });
    userService.*/
}


/*int main(int argc, char const* const* argv) {
    Chat chat;

    auto chatService = webcom::Service("chat", [&]() {
        // create access, in theory we could do an access check here
        return webcom::make<ChatViewController>(chat);
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
}*/
