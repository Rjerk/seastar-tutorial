#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/thread.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

using namespace std::chrono_literals;

namespace ss = seastar;

ss::future<> slow_incr(int i)
{
    return ss::async([i]() {
        ss::sleep(10ms).get();
        return i + 1;
    })
    .then([](int i) {
        std::cout << i << std::endl;
        return ss::make_ready_future();
    });
}

ss::future<> f()
{
    return slow_incr(1);
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
