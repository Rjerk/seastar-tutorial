#include <boost/iterator/counting_iterator.hpp>

#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/future.hh>
#include <seastar/core/gate.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sharded.hh>
#include <seastar/core/sleep.hh>
#include <seastar/net/api.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

class my_service final {
public:
    std::string _str;
    my_service(const std::string& str) : _str(str)
    {
    }

    ss::future<> run()
    {
        std::cerr << "running on " << ss::this_shard_id() << ", _str = " << _str << "\n";
        return ss::make_ready_future<>();
    }
    ss::future<> stop()
    {
        return ss::make_ready_future<>();
    }
};

ss::sharded<my_service> s;

ss::future<> f()
{
    return s.start(std::string("hello"))
        .then([] {
            return s.invoke_on_all([](my_service& local_service) {
                return local_service.run();
            });
        })
        .then([] {
            return s.stop();
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
