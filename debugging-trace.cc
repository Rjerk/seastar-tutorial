#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/backtrace.hh>

seastar::future<> g()
{
    return seastar::make_exception_future_with_backtrace<>(std::runtime_error("hello"));
}

seastar::future<> f()
{
    (void)g();
    return seastar::sleep(std::chrono::seconds(1));
}

int main(int argc, char** argv)
{
    seastar::app_template app;
    app.run(argc, argv, f);
}
