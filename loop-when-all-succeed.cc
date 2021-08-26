#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/when_all.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

ss::future<> f()
{
    return ss::when_all_succeed(ss::make_ready_future<int>(2), ss::make_ready_future<double>(3.5)).then_unpack([](int i, double d) {
        std::cout << i << " " << d << "\n";
        return ss::make_ready_future();
    });
}

ss::future<> g()
{
    return ss::when_all_succeed(ss::make_ready_future<int>(2), ss::make_exception_future<double>("oops"))
        .then_unpack([](int i, double d) {
            std::cout << i << " " << d << "\n"; // not run
        })
        .handle_exception([](std::exception_ptr e) {
            std::cout << "exception: " << e << "\n";
        });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, g);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
