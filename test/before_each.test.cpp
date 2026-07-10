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

suite("before_each") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("runs from outer suites to inner suites before each test", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            before_each([&] {
                calls.push_back("root before_each");
            });
            describe("nested", [&] {
                before_each([&] {
                    calls.push_back("nested before_each");
                });
                it("case", [&] {
                    calls.push_back("test");
                });
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{
            "root before_each",
            "nested before_each",
            "test"
        });
    });

    it("accepts prebuilt hook objects", [] {
        auto calls = std::vector<std::string>();

        define_root_suite("root", [&] {
            before_each(hook("before_each", [&] {
                calls.push_back("before_each");
            }));
            it("case", [&] {
                calls.push_back("test");
            });
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(calls, std::vector<std::string>{"before_each", "test"});
    });
}
