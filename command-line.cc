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

int main(int argc, char** argv)
{
    ss::app_template app;

    namespace bpo = boost::program_options;

    app.add_options()("flag", "some optional flag")("size,s", bpo::value<int>()->default_value(100), "size");

    app.add_positional_options({{"filename", bpo::value<std::vector<ss::sstring>>()->default_value({}), "sstable files to verify", -1}});

    app.run(argc, argv, [&app] {
        auto& args = app.configuration();

        if (args.count("flag")) {
            std::cout << "Flag is on\n";
        }
        std::cout << "Size is " << args["size"].as<int>() << "\n";

        for (auto&& fn : args["filename"].as<std::vector<ss::sstring>>()) {
            std::cout << fn << "\n";
        }

        return ss::make_ready_future<>();
    });
    return 0;
}
