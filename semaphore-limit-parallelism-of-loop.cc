#include <boost/iterator/counting_iterator.hpp>

#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/semaphore.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

ss::future<> slow()
{
    std::cerr << ".";
    return ss::sleep(1s);
}

// there is no limit to the amount of parallelism
seastar::future<> infinit()
{
    return seastar::repeat([] {
        (void)slow();
        return seastar::stop_iteration::no;
    });
}

// print 10 dots every time, infinitly
seastar::future<> f()
{
    return seastar::do_with(seastar::semaphore(10), [](auto& limit) {
        return seastar::repeat([&limit] {
            return seastar::get_units(limit, 1).then([](auto units) {
                (void)slow().finally([units = std::move(units)] {
                });
                return seastar::stop_iteration::no;
            });
        });
    });
}

// print 456 dots, every time print up to 100 dots
seastar::future<> g()
{
    return seastar::do_with(seastar::semaphore(100), [](auto& limit) {
        return seastar::do_for_each(
                   boost::counting_iterator<int>(0),
                   boost::counting_iterator<int>(456),
                   [&limit](int i) {
                       return seastar::get_units(limit, 1).then([](auto units) {
                           (void)slow().finally([units = std::move(units)] {
                           });
                       });
                   })
            .finally([&limit] {
                return limit.wait(100);
            });
    });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, g);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
