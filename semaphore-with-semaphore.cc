#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/semaphore.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

ss::future<> slow()
{
    return ss::sleep(1s);
}

seastar::future<> g()
{
    static thread_local seastar::semaphore limit(6);

    // take three units every time, so max concorrency is 2
    return seastar::with_semaphore(limit, 3, [] {
        return slow(); // do the real work of g()
    });
}

seastar::future<> parallel()
{
    auto f = seastar::parallel_for_each(std::vector<int>(10, 1), [](int i) -> seastar::future<> {
        co_await g();
        std::cout << i << "\n";
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
