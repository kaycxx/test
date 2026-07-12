# Writing Tests

`kaycxx-test` separates test registration from test execution. Suite registration happens before `main()` starts. Test bodies run later when the test runner executes the registry.

## Top-Level Suites

Use the `suite` macro at namespace scope to define a top-level suite. It is the only macro in the test DSL and exists because ordinary function calls are not allowed at namespace scope.

```cpp
#include <kaycxx/test.hpp>

using namespace kaycxx::test;

suite("string helpers") {
    it("accepts empty text", [] {
        // Test code.
    });
}
```

The block following `suite` is a generated registration function. It runs during static initialization and must not be nested inside a function or another suite.

## Nested Suites

Use `describe` inside a suite to group related tests. Nested descriptions become part of each test's full description.

```cpp
suite("calculator") {
    describe("addition", [] {
        it("adds positive numbers", [] {
            // Full description: calculator addition adds positive numbers
        });

        describe("with zero", [] {
            it("keeps the value", [] {
                // Full description: calculator addition with zero keeps the value
            });
        });
    });
}
```

The `describe` callback runs immediately while the suite tree is being registered. The `it` callback is stored and runs later during test execution.

Suites and tests execute in declaration order.

## Dynamic Test Generation

Registration is ordinary C++ code, so loops and conditions can generate tests dynamically.

```cpp
#include <format>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("squares") {
    for (auto value = 1; value <= 3; value++) {
        it(std::format("squares {}", value), [value] {
            assert_equal(square(value), value * value);
        });
    }
}
```

Values needed by a test must be captured by value or otherwise kept alive until the runner executes the test.

## Callback Lifetimes

Local variables in a `suite` or `describe` registration body are destroyed when that body returns. Capturing them by reference in a test or hook produces a dangling reference because the callback runs later.

This is invalid:

```cpp
suite("invalid lifetime") {
    auto value = 42;

    it("uses the value", [&] {
        use(value);
    });
}
```

Capture immutable test data by value:

```cpp
suite("valid lifetime") {
    auto value = 42;

    it("uses the value", [value] {
        use(value);
    });
}
```

For mutable state shared by hooks and tests, use static suite state or an owning object captured by value. [Hooks and State] describes both approaches.

[Hooks and State]: hooks-and-state.md
