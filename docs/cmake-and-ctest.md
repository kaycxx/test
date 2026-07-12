# CMake and CTest

`kaycxx-test` is a static library containing the framework implementation and the default `main()` function. A test target therefore needs only test source files, the code under test, and `kaycxx::test`.

## Test Executable

```cmake
find_package(kaycxx-test 0.0.4 CONFIG REQUIRED)

file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS
    test/*.cpp
)
list(SORT TEST_SOURCES)

add_executable(my-project-tests
    ${TEST_SOURCES}
)
target_link_libraries(my-project-tests PRIVATE
    my-project
    kaycxx::test
)
```

`kaycxx::test` exposes its dependencies transitively, including `kaycxx::assert`, `kaycxx::cli`, and `kaycxx::term`. Tests can include `<kaycxx/assert.hpp>` without linking the assertion library separately.

## Direct Test Target

A custom `test` target runs the framework directly and preserves its hierarchical report.

```cmake
add_custom_target(test
    COMMAND my-project-tests
    DEPENDS my-project-tests
    USES_TERMINAL
)
```

Run it with:

```sh
cmake --build build --target test
```

## CTest Discovery

`--write-ctest FILE` creates the specified CMake file. Inside that file, it writes one `add_test` entry for every unique full test description. Each CTest entry invokes the same executable with an exact escaped description pattern. This gives CTest per-test visibility while the direct `test` target keeps the framework's native report.

```cmake
set(TEST_DISCOVERY_FILE "${CMAKE_CURRENT_BINARY_DIR}/my-project-tests.cmake")
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/CTestTestfile.cmake" "include(\"${TEST_DISCOVERY_FILE}\" OPTIONAL)\n")

add_custom_command(TARGET my-project-tests POST_BUILD
    COMMAND my-project-tests --write-ctest "${TEST_DISCOVERY_FILE}"
    COMMENT "Discovering my-project tests"
    VERBATIM
)

add_custom_target(ctest
    COMMAND ${CMAKE_CTEST_COMMAND} --test-dir "${CMAKE_CURRENT_BINARY_DIR}"
    DEPENDS my-project-tests
    USES_TERMINAL
)
```

Run the discovered tests with:

```sh
ctest --test-dir build
```

Use verbose mode to see the framework output for every invocation:

```sh
FORCE_COLOR=1 ctest --test-dir build -V
```

## Optional Tests

Wrap the test dependency and test targets in a `BUILD_TESTING` condition when the project should also build without downloading or compiling test dependencies.

```cmake
option(BUILD_TESTING "Build unit tests" ON)

if(BUILD_TESTING)
    find_package(kaycxx-test 0.0.4 CONFIG REQUIRED)

    # Define the test executable, test target, and CTest discovery here.
endif()
```

Tests can then be disabled while configuring the project:

```sh
cmake -B build -D BUILD_TESTING=OFF
```
