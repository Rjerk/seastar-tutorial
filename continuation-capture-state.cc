#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

seastar::future<int> incr(int i)
{
    using namespace std::chrono_literals;
    return seastar::sleep(10ms).then([i] {
        return i + 1;
    });
}

seastar::future<> f()
{
    return incr(3).then([](int val) {
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
