#include <seastar/core/aligned_buffer.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/scheduling.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/sstring.hh>
#include <seastar/core/thread.hh>
#include <seastar/core/when_all.hh>
#include <seastar/core/with_scheduling_group.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

using namespace std::chrono_literals;

namespace ss = seastar;

ss::future<long> loop(int parallelism, bool& stop)
{
    return ss::do_with(0L, [parallelism, &stop](long& counter) {
        return ss::parallel_for_each(
                   boost::irange<unsigned>(0, parallelism),
                   [&stop, &counter](unsigned c) {
                       return ss::do_until(
                           [&stop] {
                               return stop;
                           },
                           [&counter] {
                               ++counter;
                               return ss::make_ready_future<>();
                           });
                   })
            .then([&counter] {
                return counter;
            });
    });
}

seastar::future<long> loop_in_sg(int parallelism, bool& stop, seastar::scheduling_group sg)
{
    return seastar::with_scheduling_group(sg, [parallelism, &stop] {
        return loop(parallelism, stop);
    });
}

seastar::future<> f()
{
    return seastar::when_all_succeed(seastar::create_scheduling_group("loop1", 200), seastar::create_scheduling_group("loop2", 100))
        .then_unpack([](seastar::scheduling_group sg1, seastar::scheduling_group sg2) {
            return seastar::do_with(false, [sg1, sg2](bool& stop) {
                (void)seastar::sleep(std::chrono::seconds(3)).then([&stop] {
                    std::cout << "Stopping...\n";
                    stop = true;
                });

                return seastar::when_all_succeed(loop_in_sg(1, stop, sg1), loop_in_sg(3, stop, sg2)).then_unpack([](long n1, long n2) {
                    std::cout << "Counters: loop1 -> " << n1 << ", loop2 -> " << n2 << "\n";
                });
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
