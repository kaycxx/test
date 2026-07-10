// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include <string>
#include <vector>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/active_registry.hpp"
#include "support/recording_reporter.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;
using namespace kaycxx::test::support;

using kaycxx::test::detail::define_root_suite;

suite("before_all") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("runs before child nodes", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            before_all([&] {
                calls.push_back("before_all");
            });
            it("case", [&] {
                calls.push_back("test");
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"before_all", "test"});
    });

    it("accepts prebuilt hook objects", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            before_all(hook("before_all", [&] {
                calls.push_back("before_all");
            }));
            it("case", [&] {
                calls.push_back("test");
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"before_all", "test"});
    });

    it("skips suite children when the hook fails", [] {
        auto body_ran = false;

        define_root_suite("root", [&] {
            before_all([] {
                throw std::runtime_error("setup failed");
            });
            it("case", [&] {
                body_ran = true;
            });
        });

        auto reporter = recording_reporter();

        assert_false(current_test_registry().run(reporter));
        assert_false(body_ran);
        assert_equal(reporter.failures.size(), 1uz);
        assert_equal(reporter.skipped_by_suite_failure, 1uz);
        assert_contain(reporter.failures.front().error_message(), "before_all hook failed: setup failed");
    });
}
