#include <fon/json.h>
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

int main(int argc, char const* const* argv) {
    auto services = webcom::Services{};

    Chat chat;

    auto userController = webcom::Controller<webcom::UserConnectionView>{};
    auto chatController = webcom::Controller<ChatView>{};

    services.addController("chat", [&](webcom::Services::SendCB _send) {
        return chatController.makeView(std::move(_send), chat);
    });

    auto expectedMessagesToBeSend = std::vector<std::string>{
R"({"action":"message","params":{"0":{"action":"init","id":0,"params":{"0":[]}}}}
)",
R"({"action":"message","params":{"0":{"action":"addMsg","id":0,"params":{"0":"uiae"}}}}
)"};
    auto uv = userController.makeView([&](Json::Value node) {
        auto actual = Json::FastWriter{}.write(node);

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

    if (!expectedMessagesToBeSend.empty()) {
        throw std::runtime_error("didn't send all expected messages");
    }
}
