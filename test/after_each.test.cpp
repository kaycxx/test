// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

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

suite("after_each") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("runs from inner suites to outer suites after each test", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            after_each([&] {
                calls.push_back("root after_each");
            });
            describe("nested", [&] {
                after_each([&] {
                    calls.push_back("nested after_each");
                });
                it("case", [&] {
                    calls.push_back("test");
                });
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{
            "test",
            "nested after_each",
            "root after_each"
        });
    });

    it("accepts prebuilt hook objects", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            after_each(hook("after_each", [&] {
                calls.push_back("after_each");
            }));
            it("case", [&] {
                calls.push_back("test");
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"test", "after_each"});
    });
}
