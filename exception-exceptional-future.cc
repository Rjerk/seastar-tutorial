#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

class my_exception : public std::exception {
    virtual const char* what() const noexcept override
    {
        return "my exception";
    }
};

void inner()
{
    throw my_exception();
}

seastar::future<> fail()
{
    try {
        inner();
    } catch (...) {
        return seastar::make_exception_future(std::current_exception());
    }
    return seastar::make_ready_future<>();
}

seastar::future<> f()
{
    return fail().finally([] {
        std::cout << "cleaning up\n";
    }); // return a exceptional future
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
