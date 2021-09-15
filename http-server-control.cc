#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/future.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/thread.hh>
#include <seastar/http/function_handlers.hh>
#include <seastar/http/httpd.hh>
#include <seastar/http/routes.hh>
#include <seastar/http/transformers.hh>
#include <seastar/net/socket_defs.hh>

namespace ss = seastar;

class stop_signal {
    bool                   _caught = false;
    ss::condition_variable _cond;

private:
    void signaled()
    {
        if (_caught) {
            return;
        }
        _caught = true;
        _cond.broadcast();
    }

public:
    stop_signal()
    {
        ss::engine().handle_signal(SIGINT, [this] {
            signaled();
        });
        ss::engine().handle_signal(SIGTERM, [this] {
            signaled();
        });
    }
    ~stop_signal()
    {
        ss::engine().handle_signal(SIGINT, [] {
        });
        ss::engine().handle_signal(SIGTERM, [] {
        });
    }
    ss::future<> wait()
    {
        return _cond.wait([this] {
            return _caught;
        });
    }
    bool stopping() const
    {
        return _caught;
    }
};

ss::future<> f()
{
    return ss::async([]() {
        stop_signal stop_signal;

        ss::httpd::http_server_control http_server;

        std::cout << ss::format("starting server\n");
        http_server.start("my_http_server").get();

        std::cout << ss::format("set_routes for server\n");
        http_server
            .set_routes([](ss::routes& r) {
                auto h1 = new ss::httpd::function_handler([](ss::httpd::const_req req) {
                    return "hello";
                });

                r.add(ss::httpd::operation_type::GET, ss::httpd::url("/"), h1);
            })
            .get();

        std::cout << ss::format("server is listening on port 8888\n");
        http_server.listen(ss::ipv4_addr("0.0.0.0", 8888))
            .handle_exception([](auto ep) {
                std::cerr << ss::format("Could not start server{}\n", ep);
                return ss::make_exception_future<>(ep);
            })
            .get();

        ss::engine().at_exit([&http_server]() {
            std::cout << ss::format("stopping server\n");
            return http_server.stop();
        });

        stop_signal.wait().get();
    });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run_deprecated(argc, argv, f);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
