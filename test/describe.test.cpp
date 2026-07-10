// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/active_registry.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;
using namespace kaycxx::test::support;

using kaycxx::test::detail::define_root_suite;

suite("describe") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("registers nested suites", [] {
        define_root_suite("root", [] {
            describe("nested", [] {
                it("case", [] {});
            });
        });

        assert_equal(current_test_registry().num_test_suites(), 2uz);
        assert_equal(current_test_registry().num_test_cases(), 1uz);
    });
}
