#include <seastar/util/defer.hh>
#include <seastar/util/log.hh>

seastar::logger startlog("init");

template <typename Func>
static auto defer_verbose_shutdown(const char* what, Func&& func)
{
    auto vfunc = [what, func = std::forward<Func>(func)]() mutable {
        startlog.info("Shutting down {}", what);
        try {
            func();
        } catch (...) {
            startlog.error("Unexpected error shutting down {}: {}", what, std::current_exception());
            throw;
        }
        startlog.info("Shutting down {} was successful", what);
    };

    auto ret = seastar::deferred_action(std::move(vfunc));
    return seastar::make_shared<decltype(ret)>(std::move(ret));
}

int main()
{
    auto end_of_the_program = defer_verbose_shutdown("stopping the program...", [] {
        std::cout << "bye.\n";
    });
}
