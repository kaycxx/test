# Skipping Tests

Pass a skip condition to `it` when a test is intentionally unavailable. Skip conditions are evaluated before `before_each` hooks and before the test body.

## Unconditional Skips

`skip` always skips the test. The optional reason is shown by the default reporter.

```cpp
suite("network") {
    it("reconnects after a timeout", skip("Reconnect support is not implemented"), [] {
        // Not executed.
    });
}
```

## Conditional Skips

`skip_if` skips when its callback returns `true`.

```cpp
suite("graphics") {
    it("uses Vulkan", skip_if([] {
        return !vulkan_available();
    }, "Vulkan is not available"), [] {
        // Vulkan test.
    });
}
```

`skip_unless` skips unless its callback returns `true`.

```cpp
suite("database") {
    it("connects to PostgreSQL", skip_unless([] {
        return postgres_available();
    }, "PostgreSQL is not available"), [] {
        // PostgreSQL test.
    });
}
```

The condition runs when the test is about to execute, not while suites are registered. This allows it to inspect the current environment.

Skipped tests do not run hooks or test code. They are shown in yellow by the default reporter, count as skipped in the summary, and do not by themselves make the test process fail.
