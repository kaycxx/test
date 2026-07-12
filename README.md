# kaycxx-test

C++ unit test framework with hierarchical suites, lifecycle hooks, skipping, test filtering, and CTest discovery.

[GitHub] | [API Documentation]

## Requirements

- C++23 compiler and standard library
- Exception support
- Run-time type information (RTTI)

## Usage

Tests are declared with one top-level `suite`, nested `describe` blocks, and `it` test cases. The linked test library provides `main()` and runs every registered suite.

```cpp
#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("calculator") {
    describe("addition", [] {
        it("adds positive numbers", [] {
            assert_equal(2 + 3, 5);
        });

        it("adds negative numbers", [] {
            assert_equal(-2 + -3, -5);
        });
    });
}
```

The example uses [kaycxx-assert] for assertions. Tests can use any other exception-based assertion library or custom assertion functions because assertions are ordinary functions that report failures by throwing exceptions. An exception that leaves a test body is reported as a failed test.

CMake users consume the installed package with:

```cmake
find_package(kaycxx-test 0.0.4 CONFIG REQUIRED)

add_executable(my-project-tests
    test/calculator.test.cpp
)
target_link_libraries(my-project-tests PRIVATE
    my-project
    kaycxx::test
)
```

No separate `main.cpp` is needed for the test executable.

Non-CMake users can use pkg-config:

```sh
c++ test/calculator.test.cpp $(pkg-config --cflags --libs kaycxx-test) -o my-project-tests
```

## Example Output

Running the test executable produces a hierarchical report. The test framework adds colors automatically when the output is connected to a supported terminal.

When both tests pass:

<img src="docs/assets/example-output-passing.svg" alt="Passing test output" width="100%" height="209">

If addition incorrectly returns `5` instead of `-5` for the negative numbers:

<img src="docs/assets/example-output-failing.svg" alt="Failing test output" width="100%" height="369">

## Guides

- [Writing Tests] explains `suite`, `describe`, `it`, registration order, dynamic test generation, and callback lifetimes.
- [Hooks and State] explains setup and teardown hooks, inherited hook order, shared state, and hook failures.
- [Skipping Tests] explains unconditional and dynamic skip conditions.
- [Running and Filtering] explains command-line filters, regular expressions, output, and exit codes.
- [CMake and CTest] explains test executables, the standard `test` target, and per-test CTest discovery.

## Build From Source

```sh
cmake --preset release
cmake --build --preset release
```

`kaycxx-test` is always built as a static library because it supplies the test executable's `main()` function.

## Install

```sh
cmake --install build/release --prefix /tmp/root
```

If no prefix is specified, CMake installs to `/usr/local` by default on Unix systems.

## Development

Run all tests:

```sh
cmake --preset debug
cmake --build --preset debug --target test
```

Generate API documentation with Doxygen:

```sh
cmake --build --preset debug --target apidoc
```

The generated HTML documentation is written to `build/debug/apidoc/html/index.html`.

[GitHub]: https://github.com/kaycxx/test
[API Documentation]: https://kaycxx.github.io/test/
[kaycxx-assert]: https://github.com/kaycxx/assert
[Writing Tests]: docs/writing-tests.md
[Hooks and State]: docs/hooks-and-state.md
[Skipping Tests]: docs/skipping-tests.md
[Running and Filtering]: docs/running-and-filtering.md
[CMake and CTest]: docs/cmake-and-ctest.md
