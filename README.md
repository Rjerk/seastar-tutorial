# Seastar Tuturial Examples

## Build Seastar

Build seastar in a scylla-toolchain container:

```bash
docker run -dti --name seastar --privileged=true --restart=always -v $PWD:$PWD -w $PWD docker.io/scylladb/scylla-toolchain:fedora-37-20221115 bash

docker exec -it seastar bash

git submodule update --init --force --recursive

cd seastar

# enable coroutines with --cflags="-fcoroutines"
./configure.py --mode=release --prefix=/usr/local --c++-standard=23 --without-apps --without-demos --cflags="-fcoroutines"

# build and install
ninja -C build/release install
```

## Build Examples

Build and run our examples:

```bash
cd .. && mkdir build && cd build

cmake3 -G Ninja ..

ninja

./get-started
```
