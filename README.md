# seastar tuturial

Seastar and GCC 10 should be ready, see [compiler and install the seastar](./seastar-install.md)

Build and run:

```bash
mkdir build && cd build

seastar_dir=/log/indexer/seastar

cmake3 -G Ninja -DCMAKE_PREFIX_PATH="$seastar_dir/build/release;$seastar_dir/build/release/_cooking/installed" -DCMAKE_MODULE_PATH=$seastar_dir/cmake ..

ninja

./get-started
```
