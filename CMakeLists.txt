cmake_minimum_required (VERSION 3.5)

project (seastar_tutorial)

find_package (Seastar REQUIRED)

set (PRIVATE_CXX_FLAGS
  -fvisibility=hidden
  -UNDEBUG
  -std=gnu++20
  -fcoroutines
  -g
  -O0
  -Wall
  -Werror
  -Wno-array-bounds # Disabled because of https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93437
  -Wno-error=deprecated-declarations)

function(seastar_example name)
    set(TEST_TARGET ${name})

    add_executable(${TEST_TARGET} ${name}.cc)
    target_compile_options(${TEST_TARGET} PRIVATE ${PRIVATE_CXX_FLAGS}) 
    target_link_libraries(${TEST_TARGET} Seastar::seastar)
endfunction()


seastar_example(get-started)

seastar_example(threads-smp-count)

seastar_example(future-continuation)
seastar_example(future-parallelism)
seastar_example(future-sleep-slow)
seastar_example(future-ready-future)

seastar_example(coroutine-example)
seastar_example(coroutine-with-exception)
seastar_example(coroutine-make-exception)
seastar_example(coroutine-with-concurrency)

seastar_example(continuation-capture-state)
seastar_example(continuation-capture-by-move)

seastar_example(exception-exceptional-future)
seastar_example(exception-handle)
seastar_example(exception-futurize-invoke)

seastar_example(lifetime-do-with)
seastar_example(lifetime-reference-counting)
seastar_example(lifetime-save-on-stack)
