// SPDX-FileCopyrightText: 2024 Simon Gene Gottlieb
// SPDX-License-Identifier: CC0-1.0

#include <catch2/catch_all.hpp>
#include <fmt/color.h>
#include <fmt/format.h>
#include <fon/json.h>
#include <fon/std/all.h>
#include <webcom/webcom.h>

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

TEST_CASE("try controller", "[webcom][controller]") {
    Chat chat;

    auto chatController = webcom::Controller<Chat&>{chat};

    auto expectedMessagesToBeSend = std::vector<std::string>{
R"({"action":"init","params":{"0":[]}}
)",
R"({"action":"addMsg","params":{"0":"uiae"}}
)"};
    auto cc = chatController.makeView<Chat::View>([&](Json::Value node) {
        auto actual = Json::FastWriter{}.write(node);

        REQUIRE(!expectedMessagesToBeSend.empty());

        CHECK(expectedMessagesToBeSend[0] == actual);
        expectedMessagesToBeSend.erase(begin(expectedMessagesToBeSend));
    }, chat);

    {
        auto msg = Json::Value{};
        msg["action"] = "addText";
        msg["params"]["0"] = "uiae";
        cc->dispatchSignalFromClient(msg);
    }

    CHECK(expectedMessagesToBeSend.empty());
}

TEST_CASE("try services", "[webcom][services]") {
    auto services = webcom::Services{};

    Chat chat;

    auto userController = webcom::Controller<Chat&>{chat};
    services.setController("chat", chat);



    auto expectedMessagesToBeSend = std::vector<std::string>{
R"({"action":"message","params":{"0":{"action":"init","id":0,"params":{"0":[]}}}}
)",
R"({"action":"message","params":{"0":{"action":"addMsg","id":0,"params":{"0":"uiae"}}}}
)"};
    auto uv = userController.makeView<webcom::UserConnectionView>([&](Json::Value node) {
        auto actual = Json::FastWriter{}.write(node);

        REQUIRE(!expectedMessagesToBeSend.empty());

        CHECK(expectedMessagesToBeSend[0] == actual);
        expectedMessagesToBeSend.erase(begin(expectedMessagesToBeSend));
    }, services);

    {
        auto msg = Json::Value{};
        msg["action"] = "subscribe";
        msg["params"]["0"] = 0;
        msg["params"]["1"] = "chat";
        uv->dispatchSignalFromClient(msg);
    }
    {
        auto msg = Json::Value{};
        msg["action"] = "message";
        msg["params"]["0"] = 0;
        auto params = Json::Value{};
        params["action"] = "addText";
        params["params"]["0"] = "uiae";
        msg["params"]["1"] = params;
        uv->dispatchSignalFromClient(msg);
    }

    CHECK(expectedMessagesToBeSend.empty());
}
