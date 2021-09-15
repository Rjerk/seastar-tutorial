#include <seastar/core/do_with.hh>
#include <seastar/core/reactor.hh>
#include <seastar/core/shared_ptr.hh>

using ss = namespace seastar;

extern "C" {
#include <sys/types.h>

#include <signal.h>
#include <unistd.h>
}

int f()
{
    return ss::do_with(ss::make_lw_shared<ss::promise<>>(), false, [](auto const& p, bool& signaled) {
        ss::engine().handle_signal(SIGHUP, [p, &signaled] {
            signaled = true;
            p->set_value();
        });

        kill(getpid(), SIGHUP);

        return p->get_future().then([&] {
            assert(signaled == true);
        });
    });
}

int main()
{
    seastar::app_template app;
    app.run(argc, argv, f);
}
