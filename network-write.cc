#include <boost/iterator/counting_iterator.hpp>

#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/future-util.hh>
#include <seastar/core/future.hh>
#include <seastar/core/gate.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/net/api.hh>
#include <seastar/util/log.hh>

namespace ss = seastar;

using namespace std::chrono_literals;

const char* canned_response = "Seastar is the future!\n";

ss::future<> service_loop()
{
    ss::listen_options lo;
    lo.reuse_address = true;
    return ss::do_with(ss::listen(ss::make_ipv4_address({1234}), lo), [](auto& listener) {
        return ss::keep_doing([&listener]() {
            return listener.accept().then([](ss::accept_result res) {
                auto s   = std::move(res.connection);
                auto out = s.output();
                return ss::do_with(std::move(s), std::move(out), [](auto& s, auto& out) {
                    return out.write(canned_response).then([&out] {
                        return out.close();
                    });
                });
            });
        });
    });
}

ss::future<> f()
{
    return ss::parallel_for_each(boost::irange<unsigned>(0, ss::smp::count), [](unsigned c) {
        return ss::smp::submit_to(c, service_loop);
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
