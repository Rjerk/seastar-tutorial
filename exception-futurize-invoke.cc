#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

seastar::future<> f()
{
    return seastar::futurize_invoke(
               [](int arg) {
                   throw "expected exception";
                   return arg;
               },
               1)
        .then_wrapped([](seastar::future<int> f) {
            try {
                f.get();
            } catch (const std::exception& e) {
                std::cout << "caught: " << e.what() << std::endl; // 这里不会打印，因为并不会捕获到一个异常
            }
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
