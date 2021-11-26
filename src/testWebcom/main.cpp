#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

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

TEST_CASE("try controller", "[webcom][controller]") {
    Chat chat;

    auto chatController = webcom::Controller<ChatView>{};

    auto expectedMessagesToBeSend = std::vector<std::string>{
        "action: init\n"
        "params:\n"
        "  0: ~",
        "action: addMsg\n"
        "params:\n"
        "  0: uiae"
    };
    auto cc = chatController.makeView([&](YAML::Node node) {
        YAML::Emitter emit;
        emit << node;
        auto actual = std::string{emit.c_str()};

        REQUIRE(!expectedMessagesToBeSend.empty());

        CHECK(expectedMessagesToBeSend[0] == actual);
        expectedMessagesToBeSend.erase(begin(expectedMessagesToBeSend));
    }, chat);

    {
        auto msg = YAML::Node{};
        msg["action"] = "addText";
        msg["params"].push_back("uiae");
        cc->dispatchSignalFromClient(msg);
    }

    CHECK(expectedMessagesToBeSend.empty());
}

TEST_CASE("try services", "[webcom][services]") {
    auto services = webcom::Services{};

    Chat chat;

    auto userController = webcom::Controller<webcom::UserConnectionView>{};
    auto chatController = webcom::Controller<ChatView>{};

    services.addController("chat", [&](webcom::Services::SendCB _send) {
        return chatController.makeView(std::move(_send), chat);
    });


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
    auto uv = userController.makeView([&](YAML::Node node) {
        YAML::Emitter emit;
        emit << node;
        auto actual = std::string{emit.c_str()};

        REQUIRE(!expectedMessagesToBeSend.empty());

        CHECK(expectedMessagesToBeSend[0] == actual);
        expectedMessagesToBeSend.erase(begin(expectedMessagesToBeSend));
    }, services);

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

    CHECK(expectedMessagesToBeSend.empty());
}
