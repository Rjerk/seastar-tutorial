#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

#include <stdexcept>

#ifndef SEASTAR_COROUTINES_ENABLED

int main(int argc, char** argv)
{
    std::cout << "coroutines not available\n";
    return 0;
}

#else

using namespace std::chrono_literals;

seastar::future<> function_returning_an_exceptional_future()
{
    throw "foo"; // fails to return a future!
}

seastar::future<> exception_handling()
{
    try {
        co_await function_returning_an_exceptional_future();
    } catch (...) {
        // exception will be handled here
    }

    // will be captured by coroutine and returned as an exceptional future
    throw 3;
}

int main(int argc, char** argv)
{
    seastar::app_template app;
    try {
        app.run(argc, argv, exception_handling);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}

#endif
