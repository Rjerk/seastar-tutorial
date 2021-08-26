#include <seastar/core/aligned_buffer.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/sstring.hh>
#include <seastar/core/thread.hh>
#include <seastar/core/when_all.hh>
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

ss::future<> f()
{
    return ss::do_with(false, [](bool& stop) {
        (void)ss::sleep(3s).then([&stop] {
            stop = true;
        });

        return ss::when_all_succeed(loop(1, stop), loop(10, stop)).then_unpack([](long n1, long n2) {
            std::cout << "Counters: " << n1 << ", " << n2 << "\n";
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
