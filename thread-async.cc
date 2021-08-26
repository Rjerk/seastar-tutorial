#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/thread.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

ss::future<> f()
{
    return seastar::async([] {
        std::cout << "Hi.\n";
        for (int i = 1; i < 4; i++) {
            seastar::sleep(std::chrono::seconds(1)).get();
            std::cout << i << "\n";
        }
    });
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
