#include <seastar/core/app-template.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

#include <iostream>
#include <stdexcept>

seastar::future<> f()
{
    std::cout << "Sleeping... " << std::flush;
    using namespace std::chrono_literals;
    (void)seastar::sleep(200ms).then([] {
        std::cout << "200ms " << std::flush;
    });
    (void)seastar::sleep(100ms).then([] {
        std::cout << "100ms " << std::flush;
    });
    return seastar::sleep(1s).then([] {
        std::cout << "Done.\n";
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
