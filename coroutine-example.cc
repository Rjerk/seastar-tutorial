#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

#include <stdexcept>

#ifndef SEASTAR_COROUTINES_ENABLED

int main(int argc, char** argv)
{
    std::cout << "coroutines not available\n";
    return 0;
}

#else

using namespace std::chrono_literals;

seastar::future<int> read()
{
    return seastar::make_ready_future<int>(10);
}

seastar::future<> write(int n)
{
    return seastar::make_ready_future<>();
}

seastar::future<int> slow_fetch_and_increment()
{
    auto n = co_await read();
    co_await seastar::sleep(1s);
    auto new_n = n + 1;
    co_await write(new_n);
    co_return n;
}

seastar::future<> f()
{
    return slow_fetch_and_increment().then([](int val) {
        std::cout << "Got " << val << "\n";
    });
}

int main(int argc, char** argv)
{
    seastar::app_template app;
    try {
        app.run(argc, argv, f);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}

#endif
