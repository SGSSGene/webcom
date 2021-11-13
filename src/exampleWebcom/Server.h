#include <webcom/webcom.h>

inline auto serveFile(std::filesystem::path file) -> cndl::OptResponse {
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
