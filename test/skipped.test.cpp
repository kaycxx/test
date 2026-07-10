// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("skipped") {
    it("defaults to an empty reason", [] {
        auto result = skipped();

        assert_equal(result.reason(), "");
    });

    it("stores the skip reason", [] {
        auto result = skipped("not today");

        assert_equal(result.reason(), "not today");
    });
}
