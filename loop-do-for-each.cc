#include <boost/iterator/counting_iterator.hpp>

#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/loop.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/shared_ptr.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

using namespace std::chrono_literals;

namespace ss = seastar;

ss::future<> append(ss::lw_shared_ptr<std::vector<int>> queue2, ss::lw_shared_ptr<std::vector<int>> queue1)
{
    // future<> do_for_each(Container& c, AsyncAction action);
    return ss::do_for_each(*queue1, [queue2](int element) {
        queue2->push_back(element);
    });
}

ss::future<> append_iota(ss::lw_shared_ptr<std::vector<int>> queue, int n)
{
    // future<> do_for_each(Iterator begin, Iterator end, AsyncAction action);
    return ss::do_for_each(boost::make_counting_iterator<int>(0), boost::make_counting_iterator<int>(n), [queue](int element) {
        queue->push_back(element);
    });
}

ss::future<> loop()
{
    ss::lw_shared_ptr<std::vector<int>> queue1 = ss::make_lw_shared<std::vector<int>>();
    ss::lw_shared_ptr<std::vector<int>> queue2 = ss::make_lw_shared<std::vector<int>>();

    return append_iota(queue1, 10)
        .then([queue1]() {
            std::cout << "after append queue1 size: " << queue1->size() << std::endl;
        })
        .then([queue1, queue2]() {
            return append(queue2, queue1).then([queue2]() {
                std::cout << "after append queue2 size: " << queue2->size() << std::endl;
            });
        });
}

int main(int argc, char** argv)
{
    ss::app_template app;
    try {
        app.run(argc, argv, loop);
    } catch (...) {
        std::cerr << "Couldn't start application: " << std::current_exception() << "\n";
        return 1;
    }
    return 0;
}
