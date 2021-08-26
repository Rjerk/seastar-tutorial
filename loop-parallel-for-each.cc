#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

ss::future<> parallel_print()
{
    auto f = seastar::parallel_for_each(std::vector<int>{1, 2, 3, 4, 5, 6, 7, 8, 9, 10}, [](int i) -> seastar::future<> {
        co_await seastar::sleep(std::chrono::seconds(i));
        std::cout << i << "\n";
    });
    co_await std::move(f);
}

ss::future<> f()
{
    return parallel_print();
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, f);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
