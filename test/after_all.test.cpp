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

suite("after_all") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("runs after child nodes", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            it("case", [&] {
                calls.push_back("test");
            });
            after_all([&] {
                calls.push_back("after_all");
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"test", "after_all"});
    });

    it("accepts prebuilt hook objects", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            it("case", [&] {
                calls.push_back("test");
            });
            after_all(hook("after_all", [&] {
                calls.push_back("after_all");
            }));
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"test", "after_all"});
    });

    it("reports hook failures without marking tests as skipped", [] {
        auto body_ran = false;

        define_root_suite("root", [&] {
            it("case", [&] {
                body_ran = true;
            });
            after_all([] {
                throw std::runtime_error("teardown failed");
            });
        });

        auto reporter = recording_reporter();

        assert_false(current_test_registry().run(reporter));
        assert_true(body_ran);
        assert_equal(reporter.failures.size(), 1uz);
        assert_equal(reporter.skipped_by_suite_failure, 0uz);
        assert_contain(reporter.failures.front().error_message(), "after_all hook failed: teardown failed");
    });
}
