#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/sleep.hh>
#include <seastar/coroutine/all.hh>
#include <seastar/util/log.hh>

seastar::future<int> read(int key)
{
    return seastar::make_ready_future<int>(key);
}

seastar::future<int> parallel_sum(int key1, int key2)
{
    auto [a, b] = co_await seastar::coroutine::all(
        [&] {
            return read(key1);
        },
        [&] {
            return read(key2);
        });

    co_return a + b;
}

seastar::future<> f()
{
    auto return_future = parallel_sum(1, 2);

    std::cout << return_future.get() << std::endl;

    return seastar::make_ready_future();
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
