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
    ss::future<int> slow_two = ss::sleep(2s).then([] {
        return 2;
    });
    // wait up to 2s, so all futures are ready
    return ss::when_all(ss::sleep(1s), std::move(slow_two), ss::make_ready_future<double>(3.5)).discard_result();
}

ss::future<> g()
{
    ss::future<int> slow_two = ss::sleep(2s).then([] {
        return 2;
    });
    // The future returned by when_all() resolves to a tuple of futures which are already resolved
    return ss::when_all(ss::sleep(1s), std::move(slow_two), ss::make_ready_future<double>(3.5)).then([](auto tup) {
        std::cout << std::get<0>(tup).available() << "\n";
        std::cout << std::get<1>(tup).get0() << "\n";
        std::cout << std::get<2>(tup).get0() << "\n";
    });
}

ss::future<> h()
{
    ss::future<> slow_success   = ss::sleep(1s);
    ss::future<> slow_exception = ss::sleep(2s).then([] {
        throw 1;
    });

    // inconvenient and verbose to use properly
    return ss::when_all(std::move(slow_success), std::move(slow_exception)).then([](auto tup) {
        std::cout << std::get<0>(tup).available() << "\n";
        std::cout << std::get<1>(tup).failed() << "\n";
        std::get<1>(tup).ignore_ready_future();
    });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, h);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
