#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

int do_something(std::unique_ptr<int> obj)
{
    // do some computation based on the contents of obj
    return *obj + 17;
    // at this point, obj goes out of scope so the compiler delete()s it.
}

seastar::future<int> slow_do_something(std::unique_ptr<int> obj)
{
    using namespace std::chrono_literals;
    return seastar::sleep(10ms).then([obj = std::move(obj)]() mutable {
        return do_something(std::move(obj));
    });
}

seastar::future<> f()
{
    auto i = std::make_unique<int>(1);

    return slow_do_something(std::move(i)).then([](int val) {
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
