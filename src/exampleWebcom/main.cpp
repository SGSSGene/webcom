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

auto serveFile(std::filesystem::path file) -> cndl::OptResponse {
    auto ifs = std::ifstream(file.string(), std::ios::binary);
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    auto response = cndl::Response{buffer.str()};
    if (file.extension() == ".css") {
        response.fields.emplace("Content-Type", "text/css; charset=utf-8");
        return response;
    } else if (file.extension() == ".js") {
        response.fields.emplace("Content-Type", "text/javascript; charset=utf-8");
        return response;
    } else if (file.extension() == ".png") {
        response.fields.emplace("Content-Type", "image/png");
        return response;
    } else {
        response.fields.emplace("Content-Type", "text/html; charset=utf-8");
        return response;
    }
    std::cout << "not found " << file << "\n";
    return std::nullopt;
};

struct Server {
    simplyfile::Epoll epoll;
    cndl::Server cndlServer{epoll};

    using Route = cndl::Route<cndl::OptResponse(cndl::Request const&, std::string_view)>;
    Route route1;
    Route route2;

    cndl::Route<cndl::OptResponse(cndl::Request const&)> defaultRoute;

    Server()
        : defaultRoute{std::regex{R"(/)"}, [this](cndl::Request const&) -> cndl::OptResponse {
            return serveFile("../share/exampleWebcom/index.html");
        }, {.methods={"GET"}}}
        , route1{std::regex{"/webcom/(.*)"}, [this](cndl::Request const&, std::string_view arg) -> cndl::OptResponse {
            return serveFile(std::filesystem::path{"../share/webcom"} / std::string{arg});
        }, Route::Options{.methods={"GET"}}}
        , route2{std::regex{"/(.*)"}, [this](cndl::Request const&, std::string_view arg) -> cndl::OptResponse {
            return serveFile(std::filesystem::path{"../share/exampleWebcom"} / std::string{arg});
        }, Route::Options{.methods={"GET"}}}
    {
        cndlServer.getDispatcher().addRoute(defaultRoute);
        cndlServer.getDispatcher().addRoute(route1);
        cndlServer.getDispatcher().addRoute(route2);
    }

    void run() {
        for (auto const& h : simplyfile::getHosts("127.0.0.1", std::to_string(8080))) {
            cndlServer.listen(h);
        }
        while(true) {
            epoll.work();
        }
    }
};

int main(int argc, char const* const* argv) {
    Server server;

    // Some magic container providing the web server
    auto cndlServices = webcom::CndlServices{server.cndlServer, "/ws"};

    Chat chat;
    cndlServices.provideViewController("chat", [&]() {
        // create access, in theory we could do an access check here
        return webcom::make<ChatViewController>(chat);
    });

    server.run();
}
