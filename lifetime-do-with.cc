#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

using namespace std::chrono_literals;

seastar::future<> slow_op(int o)
{
    return seastar::sleep(10ms)
        .then([o = std::move(o)] {
            return std::move(o);
        })
        .then([](int o) {
            std::cout << o << std::endl;
        });
}

seastar::future<> f()
{
    return seastar::do_with(1, [](auto& obj) {
        // obj is passed by reference to slow_op, and this is fine:
        return slow_op(obj);
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
