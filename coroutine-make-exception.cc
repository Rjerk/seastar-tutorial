#include <seastar/core/app-template.hh>
#include <seastar/core/coroutine.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

#include <iostream>
#include <stdexcept>

#ifndef SEASTAR_COROUTINES_ENABLED

int main(int argc, char** argv)
{
    std::cout << "coroutines not available\n";
    return 0;
}

#else

void foo()
{
    throw "foo"; // throw an exception instead of returning a failed future
}

// return a failed future
seastar::future<> function_returning_an_exceptional_future()
{
    return seastar::futurize_invoke(foo).finally([] {
        std::cout << "do some clean up for failed foo\n";
    });
}

seastar::future<int> exception_propagating()
{
    std::exception_ptr eptr;
    try {
        std::cout << "1\n";
        co_await function_returning_an_exceptional_future();

        std::cout << "never here\n";
    } catch (...) {
        std::cout << "2\n";
        eptr = std::current_exception();
    }

    if (eptr) {
        std::cout << "3\n";
        co_return seastar::coroutine::exception(eptr); // Saved exception pointer can be propagated without rethrowing
        // co_return seastar::coroutine::make_exception(3); // Custom exceptions can be propagated without throwing

        std::cout << "never here\n";
    }
}

seastar::future<> f()
{
    (void)exception_propagating();

    std::cout << "never here\n";
    return seastar::make_ready_future();
}

int main(int argc, char** argv)
{
    seastar::app_template app;
    try {
        app.run(argc, argv, exception_propagating);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}

#endif
