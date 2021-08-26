#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>

class myexception {
};

seastar::future<> g()
{
    return seastar::make_exception_future<>(myexception());
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
