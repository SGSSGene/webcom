#include <fon/yaml.h>
#include <fon/std/all.h>
#include <webcom/webcom.h>

#include <fmt/color.h>
#include <fmt/format.h>

/**
 * A very simple chat....it is just a list with a lock
 **/
using Chat = webcom::GuardedType<std::vector<std::string>>;

/** This represents the View (MVC) of a single User accessing the chat
 *
 * Each user (connection via websocket) will have its own view
 */
struct ChatView : webcom::View<int> {
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
    auto services = webcom::Services{};

    Chat chat;

    auto userController = webcom::Controller{[&]() {
        // create access, in theory we could do an access check here
        return webcom::make<webcom::UserConnectionView>(services);
    }};
    auto chatController = webcom::Controller{[&]() {
        return webcom::make<ChatView>(chat);
    }};

/*    auto& userController = services.makeController("services", [&]() {
        // create access, in theory we could do an access check here
        return webcom::make<webcom::UserConnectionView>(services);
    });
    auto& chatController = services.makeController("chat", [&]() {
        return webcom::make<ChatView>(chat);
    });*/

    services.addController("services", userController);
    services.addController("chat", chatController);


    auto expectedMessagesToBeSend = std::vector<std::string>{
        "action: message\n"
        "params:\n"
        "  0:\n"
        "    action: init\n"
        "    params:\n"
        "      0: ~\n"
        "    id: 0",
        "action: message\n"
        "params:\n"
        "  0:\n"
        "    action: addMsg\n"
        "    params:\n"
        "      0: uiae\n"
        "    id: 0",
    };
    auto uv = userController.createView([&](YAML::Node node) {
        YAML::Emitter emit;
        emit << node;
        auto actual = std::string{emit.c_str()};

        if (expectedMessagesToBeSend.empty()) {
            fmt::print("got:\n{}\n---\n", actual);
            throw std::runtime_error("didn't expect any message to be send");
        }
        if (expectedMessagesToBeSend[0] != actual) {
            fmt::print("expected:\n{}\n---\n", expectedMessagesToBeSend[0]);
            fmt::print("got:\n{}\n---\n", actual);
            throw std::runtime_error("unexpected message");
        }
        expectedMessagesToBeSend.erase(begin(expectedMessagesToBeSend));
    });

    {
        auto msg = YAML::Node{};
        msg["action"] = "subscribe";
        msg["params"].push_back(0);
        msg["params"].push_back("chat");
        uv->dispatchSignalFromClient(msg);
    }
    {
        auto msg = YAML::Node{};
        msg["action"] = "message";
        msg["params"].push_back(0);
        auto params = YAML::Node{};
        params["action"] = "addText";
        params["params"].push_back("uiae");
        msg["params"].push_back(params);
        uv->dispatchSignalFromClient(msg);
    }

    if (!expectedMessagesToBeSend.empty()) {
        throw std::runtime_error("didn't send all expected messages");
    }
}
