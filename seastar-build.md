# Seastar Build

+ platform: CentOS7
+ Boost 1.72.0 is already installed.

Install the dependencies:

```
./install-dependencies.sh

# cmake --version
cmake3 version 3.17.5
```

C++20 support is needed, just install GCC 10:

```bash
yum install devtoolset-10
source /opt/rh/devtoolset-10/enable
```

Get seastar and build:

```
git clone https://github.com/scylladb/seastar
git checkout seastar-20.05-branch

# enable coroutines and only compile seastar library
./configure.py --mode=release --prefix=/usr/local --c++-dialect=gnu++20 --cook c-ares --cook yaml-cpp --cook hwloc --cook liblz4 --cook gnutls --enable-experimental-coroutines-ts --without-tests --without-apps --without-demos

# build
ninja -C build/release
```

## Error Fixes

1. compiler option error:

```
g++: error: unrecognized command-line option ‘-fcoroutines-ts’; did you mean ‘-fcoroutines’?
```

FIX: `-fcoroutines-ts` is a clang compiler option, replace it with `-fcoroutines` in CMakeLists.txt

```
diff --git a/CMakeLists.txt b/CMakeLists.txt
index 5f34fe8..0d0aad9 100644
--- a/CMakeLists.txt
+++ b/CMakeLists.txt
@@ -763,7 +763,7 @@ if (MaybeUninitialized_FOUND)
 endif ()

 if (Seastar_EXPERIMENTAL_COROUTINES_TS)
-  target_compile_options (seastar PUBLIC -fcoroutines-ts -g)
+  target_compile_options (seastar PUBLIC -fcoroutines -g)
   target_compile_definitions (seastar PUBLIC SEASTAR_COROUTINES_TS)
 endif ()
```

2. compile seastar error:

```
/usr/include/linux/if.h:211:19: error: field ‘ifru_addr’ has incomplete type ‘sockaddr’
```

FIX: add a new line `#include <sys/socket.h>` in `src/net/posix-stack.cc` (before `#include <linux/if.h>`):

```
diff --git a/src/net/posix-stack.cc b/src/net/posix-stack.cc
index e0bd503..06ebe01 100644
--- a/src/net/posix-stack.cc
+++ b/src/net/posix-stack.cc
@@ -21,6 +21,7 @@

 #include <random>

+#include <sys/socket.h>
 #include <linux/if.h>
 #include <linux/netlink.h>
 #include <linux/rtnetlink.h>
```

3. using seastar couroutine but get compile error:

```
/log/seastar/include/seastar/core/coroutine.hh: At global scope:
/log/seastar/include/seastar/core/coroutine.hh:37:46: error: ‘std::experimental::coroutine_handle’ has not been declared
   37 | #define SEASTAR_INTERNAL_COROUTINE_NAMESPACE std::experimental
```

FIX: Because coroutine is moved to namespace `std::__n4861` instead of `std::experimental`, fix this:

```
diff --git a/include/seastar/core/coroutine.hh b/include/seastar/core/coroutine.hh
index 5b45587..3ce40fc 100644
--- a/include/seastar/core/coroutine.hh
+++ b/include/seastar/core/coroutine.hh
@@ -34,7 +34,7 @@
 #include <seastar/core/std-coroutine.hh>

 #ifdef SEASTAR_COROUTINES_TS
-#define SEASTAR_INTERNAL_COROUTINE_NAMESPACE std::experimental
+#define SEASTAR_INTERNAL_COROUTINE_NAMESPACE std::__n4861
 #else
 #define SEASTAR_INTERNAL_COROUTINE_NAMESPACE std
 #endif
```

4. run a seastar app error:

```
./a.out: error while loading shared libraries: libboost_program_options.so.1.72.0: cannot open shared object file: No such file or directory
```

FIX: import your Boost libraray can fix this:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/log/boost_1_72_0/lib
./a.out
```
