#include <seastar/core/aligned_buffer.hh>
#include <seastar/core/app-template.hh>
#include <seastar/core/do_with.hh>
#include <seastar/core/file.hh>
#include <seastar/core/future.hh>
#include <seastar/core/seastar.hh>
#include <seastar/core/sleep.hh>
#include <seastar/core/sstring.hh>
#include <seastar/core/thread.hh>
#include <seastar/util/log.hh>
#include <seastar/util/tmp_file.hh>

using namespace std::chrono_literals;

namespace ss = seastar;

ss::future<> read_file(ss::sstring file_name)
{
    return ss::async([file_name = std::move(file_name)]() { // lambda executed in a thread
               auto rbuf = ss::allocate_aligned_buffer<unsigned char>(4096, 4096);

               ss::file f     = seastar::open_file_dma(file_name, ss::open_flags::ro).get0(); // get0() call "blocks"
               auto     fsize = f.dma_read(0, rbuf.get(), 1024).get0();                       // "block" again
               return seastar::sstring((const char*)rbuf.get(), fsize);

           })
        .then([](ss::sstring content) {
            std::cout << content << std::endl;
        });
}

ss::future<> f()
{
    return read_file("/var/log/yum.log");
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
