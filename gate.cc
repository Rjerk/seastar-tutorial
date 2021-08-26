#include <boost/iterator/counting_iterator.hpp>

#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future.hh>
#include <seastar/core/gate.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

seastar::future<> slow(int i)
{
    std::cerr << "starting " << i << "\n";
    return seastar::sleep(8s).then([i] {
        std::cerr << "done " << i << "\n";
    });
}

seastar::future<> f()
{
    return seastar::do_with(seastar::gate(), [](auto& g) {
        return seastar::do_for_each(
                   boost::counting_iterator<int>(1),
                   boost::counting_iterator<int>(6),
                   [&g](int i) {
                       (void)seastar::with_gate(g, [i] {
                           return slow(i);
                       });
                       // wait one second before starting the next iteration
                       return seastar::sleep(1s);
                   })
            .then([&g] {
                (void)seastar::sleep(3s).then([&g] {
                    // This will fail, because it will be after the close()
                    (void)seastar::with_gate(g, [] {
                        return slow(6);
                    });
                });
                return g.close();
            });
    });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, f);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
