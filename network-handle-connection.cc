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

ss::future<> handle_connection(ss::connected_socket s, ss::socket_address a)
{
    auto out = s.output();
    auto in  = s.input();
    return do_with(std::move(s), std::move(out), std::move(in), [](auto& s, auto& out, auto& in) {
        return ss::repeat([&out, &in] {
                   return in.read().then([&out](auto buf) {
                       if (buf) {
                           return out.write(std::move(buf))
                               .then([&out] {
                                   return out.flush();
                               })
                               .then([] {
                                   return ss::stop_iteration::no;
                               });
                       } else {
                           return ss::make_ready_future<ss::stop_iteration>(ss::stop_iteration::yes);
                       }
                   });
               })
            .then([&out] {
                return out.close();
            });
    });
}

ss::future<> service_loop()
{
    ss::listen_options lo;
    lo.reuse_address = true;
    return ss::do_with(ss::listen(ss::make_ipv4_address({1234}), lo), [](auto& listener) {
        return ss::keep_doing([&listener]() {
            return listener.accept().then([](ss::accept_result res) {
                // Note we ignore, not return, the future returned by
                // handle_connection(), so we do not wait for one
                // connection to be handled before accepting the next one.
                (void)handle_connection(std::move(res.connection), std::move(res.remote_address)).handle_exception([](std::exception_ptr ep) {
                    fmt::print(stderr, "Could not handle connection: {}\n", ep);
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
