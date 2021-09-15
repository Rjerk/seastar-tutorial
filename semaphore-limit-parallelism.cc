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
    return ss::sleep(2s);
}

// limit the number of instance g()
seastar::future<> g()
{
    static thread_local seastar::semaphore limit(3);

    return limit.wait(1)
        .then([] {
            return slow(); // do the real work of g()
        })
        .finally([] {
            limit.signal(1);
        });
}

// stuck forever
seastar::future<> buggy_usage()
{
    static thread_local seastar::semaphore limit(3);

    static thread_local auto bad_slow = []() -> ss::future<> {
        throw 1;
    };

    return limit.wait(1).then([] {
        // if slow() throws an exception, the finally() will never be reached,
        // and the counter will never be increased back.
        // return bad_slow().finally([] { limit.signal(1); });

        // FIX
        return ss::futurize_invoke(bad_slow).finally([] {
            limit.signal(1);
        });
    });
}

seastar::future<> parallel()
{
    auto f = seastar::parallel_for_each(std::vector<int>(10, 1), [](int i) -> seastar::future<> {
        // co_await g();
        co_await buggy_usage();
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

#endif
