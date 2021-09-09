# Seastar Build

+ platform: CentOS 8
+ Boost 1.72.0 is already installed.

Install Boost with:

```
./bootstrap.sh --prefix=/usr/local/ --with-libraries=atomic,chrono,date_time,filesystem,program_options,system,test,thread
./b2 install
```

Install the dependencies of Seastar:

```
./install-dependencies.sh

# cmake --version
cmake3 version 3.17.5
```

C++20 support is needed, I choose the GCC 10:

```bash
yum install devtoolset-10
source /opt/rh/devtoolset-10/enable
```

Get seastar and build:

```
git submodule update --init --force --recursive

# enable coroutines with --cflags="-fcoroutines"
./configure.py --mode=release --prefix=/usr/local --c++-dialect=gnu++20 --without-apps --without-demos --cflags="-fcoroutines"

# build and install
ninja -C build/release install
```
