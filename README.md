# kaycxx-test

C++ unit test framework.

[GitHub] | [API Documentation]

## Usage

```cpp
#include <kaycxx/test.hpp>
#include <kaycxx/assert.hpp>

using namespace kaycxx::test;
using namespace kaycxx::assert;

int main() {

    return kaycxx::library_template::sum(1, 2);
}
```

CMake users consume the installed package with:

```cmake
find_package(kaycxx-library-template 1.0.0 CONFIG REQUIRED)
target_link_libraries(my-target PRIVATE kaycxx::library-template)
```

Non-CMake users can use pkg-config:

```sh
c++ $(pkg-config --cflags kaycxx-library-template) -c main.cpp
c++ main.o $(pkg-config --libs kaycxx-library-template)
```

## Build From Source

```sh
cmake -S . -B build
cmake --build build
```

A shared library is built by default. For a static build:

```sh
cmake -S . -B build -D BUILD_SHARED_LIBS=OFF
cmake --build build
```

## Install

```sh
cmake --install build --prefix /tmp/root
```

If no prefix is specified, CMake installs to `/usr/local` by default on Unix systems.

## Development

Run all tests:

```sh
cmake --build build --target test
```

Generate API documentation with Doxygen:

```sh
cmake --build build --target apidoc
```

The generated HTML documentation is written to `build/apidoc/html/index.html`.

[GitHub]: https://github.com/kaycxx/library-template
[API Documentation]: https://kaycxx.github.io/library-template/
