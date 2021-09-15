#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/semaphore.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

#ifndef SEASTAR_COROUTINES_ENABLED

int main(int argc, char** argv)
{
    std::cout << "coroutines not available\n";
    return 0;
}

#else

using namespace std::chrono_literals;

ss::future<> slow()
{
    return ss::sleep(1s);
}

seastar::future<> g()
{
    static thread_local seastar::semaphore limit(6);

    // When the units object is moved into a continuation, no matter how this continuation ends,
    // when the continuation is destructed, the units object is destructed and the units are returned
    // to the semaphore's counter
    return seastar::get_units(limit, 3).then([](auto units) {
        return slow().finally([units = std::move(units)] {
            std::cout << "releasing " << units.count() << " units\n";
        });
    });
}

seastar::future<> parallel()
{
    auto f = seastar::parallel_for_each(std::vector<int>(10, 1), [](int i) -> seastar::future<> {
        co_await g();
    });
    co_await std::move(f);
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, parallel);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}

#endif
