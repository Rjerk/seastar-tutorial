#include <seastar/core/app-template.hh>
#include <seastar/core/future.hh>
#include <seastar/core/sleep.hh>
#include <seastar/util/log.hh>

seastar::future<> f()
{
    auto finally1 = std::make_shared<bool>();
    auto finally2 = std::make_shared<bool>();

    seastar::promise<> p;
    auto               f = p.get_future()
                 .finally([=] {
                     *finally1 = true;
                     std::cout << "finally1" << std::endl;
                 })
                 .then([] {
                     throw std::runtime_error("there's a fire starting in my head");
                 })
                 .finally([=] {
                     *finally2 = true;
                     std::cout << "finally2" << std::endl;
                 })
                 .then([] {
                     std::cout << "something happened..." << std::endl; // exceptional future
                 })
                 .then_wrapped([=](auto&& f) { // trap the exception
                     try {
                         [[maybe_unused]] auto v = f.get();
                     } catch (std::exception& e) {
                         std::cout << "I see: " << e.what() << std::endl;
                     }
                 })
                 .then([] {
                     std::cout << "but keep going..." << std::endl;
                 })
                 .finally([] {
                     std::cout << "finally3" << std::endl;
                 });

    p.set_value();
    return f;
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
