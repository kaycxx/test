// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("skip") {
    it("creates unconditional skip conditions", [] {
        auto condition = skip("disabled");

        assert_true(condition.should_skip());
        assert_equal(condition.reason(), "disabled");
    });

    it("skips when skip_if condition returns true", [] {
        auto condition = skip_if([] {
            return true;
        }, "missing service");

        assert_true(condition.should_skip());
        assert_equal(condition.reason(), "missing service");
    });

    it("does not skip when skip_if condition returns false", [] {
        auto condition = skip_if([] {
            return false;
        });

        assert_false(condition.should_skip());
        assert_equal(condition.reason(), "");
    });

    it("skips when skip_unless condition returns false", [] {
        auto condition = skip_unless([] {
            return false;
        }, "unsupported platform");

        assert_true(condition.should_skip());
        assert_equal(condition.reason(), "unsupported platform");
    });

    it("does not skip when skip_unless condition returns true", [] {
        auto condition = skip_unless([] {
            return true;
        });

        assert_false(condition.should_skip());
        assert_equal(condition.reason(), "");
    });
}
