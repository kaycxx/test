// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("hook") {
    it("runs the callback", [] {
        auto called = false;
        auto value = hook("custom", [&] {
            called = true;
        });

        value.run();

        assert_true(called);
    });

    it("wraps standard exceptions with hook name", [] {
        auto value = hook("custom", [] {
            throw std::runtime_error("boom");
        });

        assert_throw<std::logic_error>([&] {
            value.run();
        }, "custom hook failed: boom");
    });

    it("wraps unknown exceptions with hook name", [] {
        auto value = hook("custom", [] {
            throw 123;
        });

        assert_throw<std::logic_error>([&] {
            value.run();
        }, "custom hook failed");
    });
}
