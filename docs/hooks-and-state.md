# Hooks and State

Hooks prepare and clean up test state at suite boundaries or around individual test cases. Hooks registered in a suite also apply to tests in nested suites.

## Hook Types

- `before_all` runs once before the selected children of its suite.
- `after_all` runs once after the selected children of its suite.
- `before_each` runs before every selected test in its suite and nested suites.
- `after_each` runs after every selected test in its suite and nested suites.

```cpp
#include <kaycxx/test.hpp>

using namespace kaycxx::test;

suite("database") {
    before_all([] {
        connect_database();
    });

    before_each([] {
        begin_transaction();
    });

    after_each([] {
        rollback_transaction();
    });

    after_all([] {
        disconnect_database();
    });

    it("stores a record", [] {
        // Test code.
    });
}
```

## Nested Hook Order

`before_each` hooks run from the outermost suite to the innermost suite. `after_each` hooks run in the opposite direction.

For this hierarchy:

```text
outer before_each
    inner before_each
        test
    inner after_each
outer after_each
```

Multiple hooks of the same type in one suite run in registration order.

## Shared State

The registration body finishes before any test runs, so ordinary local variables cannot be captured by reference. A simple suite-local state can be static and reset in hooks.

```cpp
#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("counter") {
    static auto value = 0;

    before_each([] {
        value = 0;
    });

    it("increments", [] {
        value++;
        assert_equal(value, 1);
    });
}
```

An owning value can also be captured by value in every callback that needs it.

```cpp
#include <memory>

suite("service") {
    auto state = std::make_shared<service_state>();

    before_each([state] {
        state->reset();
    });

    it("starts idle", [state] {
        check_idle(*state);
    });
}
```

## Hook Failures

Exceptions from `before_each` and `after_each` fail the current test. Exceptions from the test body also fail the current test. Execution of that test stops at the exception, so later hooks in the same test pipeline are not called.

When `before_all` fails, the suite fails and all selected tests in that suite and its nested suites are reported as skipped. Child tests, child suites, and `after_all` hooks in the failed suite do not run.

When `after_all` fails, the already completed tests keep their results and the suite is additionally reported as failed.

Hook failures point to the source location where the hook was registered. `kaycxx::assert::assertion_error` retains its own assertion location when available.
