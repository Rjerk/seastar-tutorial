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

ss::future<int> recompute_number(int number)
{
    return ss::make_ready_future<int>(number + 1);
}

ss::future<> push_until_10(ss::lw_shared_ptr<std::vector<int>> queue, int element)
{
    return ss::do_until(
        [queue] {
            std::cout << "queue size " << queue->size() << std::endl;
            return queue->size() == 10;
        }, // the first parameter is a functor return bool tell us how to stop
        [queue, element] {
            return recompute_number(element).then([queue](int new_element) {
                queue->push_back(new_element);
                std::cout << "new size is " << queue->size() << "\n";
            });
        });
}

ss::future<> loop()
{
    ss::lw_shared_ptr<std::vector<int>> queue = ss::make_lw_shared<std::vector<int>>();
    return push_until_10(queue, 1);
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
