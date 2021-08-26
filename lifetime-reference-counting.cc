#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

using namespace std::chrono_literals;

namespace ss = seastar;

// A file object can be copied, but copying does not involve copying the file descriptor
ss::future<uint64_t> slow_size(ss::file f)
{
    return ss::sleep(100ms).then([f] {
        return f.size();
    });
}

ss::future<> f()
{
    return ss::tmp_dir::do_with_thread([](ss::tmp_dir& t) {
        auto st = ss::with_file(ss::open_file_dma("/var/log/yum.log", ss::open_flags::ro), slow_size).get0();
        std::cout << "size: " << st << std::endl;
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
