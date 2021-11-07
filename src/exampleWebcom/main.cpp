#include <fon/yaml.h>
#include <fon/std/all.h>
#include <webcom/webcom.h>

#include <fmt/color.h>
#include <fmt/format.h>

/** Data Type thas must be locked to be accessible
 */
template <typename T>
class GuardedType {
    mutable std::mutex mutex;
    T          value;
public:
    auto operator*()&  {
        return std::tuple<std::lock_guard<std::mutex>, T&>{mutex, value};
    }
    auto operator*() const& {
        return std::tuple<std::lock_guard<std::mutex>, T const&>{mutex, value};
    }
};

/**
 * A very simple chat....it is just a list with a lock
 **/
using Chat = GuardedType<std::vector<std::string>>;

/** This represents the Controller and View (MVC) of a single User accessing the chat
 *
 * Each user (connection via websocket) will have its own view
 */
struct UserView {
    webcom::Adapter& adapter;
    Chat& chat;

    UserView(webcom::Adapter& _adapter, Chat& _chat)
        : adapter{_adapter}
        , chat{_chat}
    {
        auto&& [g, list] = *chat;
        // call 'init' of only this client
        adapter.callBack("init")(list);
    }

    static constexpr void reflect(auto& visitor) {
        // function that can be called by the client (webbrowser)
        visitor("addText", &UserView::addText);
    }

    void addText(std::string str) {
        auto&& [g, list] = *chat;
        list.push_back(str);
        // call 'addMsg' of all clients
        adapter.callAll("addMsg")(str);
    }
};


int main(int argc, char const* const* argv) {
    int const port = 9001;

    fmt::print("open any of\n");
    if (port != 80) {
        fmt::print("  - {}\n", format(fg(fmt::color::green), "http://{}:{}/", "localhost", port));
    } else {
        fmt::print("  - {}\n", format(fg(fmt::color::green), "http://{}/", "localhost"));
    }

    // Some magic container providing the webserver
    auto webServices = webcom::WebServices{"../share/exampleWebcom/index.html", {{"/webcom/(.*)", "../share/webcom"}, {"/(.*)", "../share/exampleWebcom"}}};

    Chat chat;

    auto serviceList = webServices.emplace("chat", [&](webcom::Adapter& adapter) {
        return adapter.make<UserView>(adapter, chat);
    });

    simplyfile::Epoll epoll;
    auto& serverEpoll = webServices.listen(simplyfile::getHosts("0.0.0.0", std::to_string(port))).getEpoll();
    epoll.addFD(serverEpoll, [&](int) {
        serverEpoll.work();
        epoll.modFD(serverEpoll, EPOLLIN | EPOLLONESHOT);
    }, EPOLLIN | EPOLLONESHOT);

    while(true) {
        epoll.work();
    }




}
