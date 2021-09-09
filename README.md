# seastar tuturial

Code example in [seastar tutorial](https://github.com/scylladb/seastar/blob/master/doc/tutorial.md).

## Build

Seastar and C++20 compiler should be ready, see [build Seastar](./seastar-build.md)

Build and run:

```bash
mkdir build && cd build

export CC=/usr/local/bin/gcc
export CXX=/usr/local/bin/g++

seastar_dir=/log/seastar

cmake3 -G Ninja -DCMAKE_PREFIX_PATH="$seastar_dir/build/release/_cooking/installed" -DCMAKE_MODULE_PATH=$seastar_dir/cmake ..

ninja

./get-started
```
