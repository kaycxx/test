// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <string>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/active_registry.hpp"
#include "support/recording_reporter.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;
using namespace kaycxx::test::support;

using kaycxx::test::detail::define_root_suite;

suite("it") {
    before_each(create_active_registry);
    after_each(destroy_active_registry);
    after_all(destroy_active_registry);

    it("reports passed test cases", [] {
        define_root_suite("root", [] {
            it("passing case", [] {});
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(reporter.events, std::vector<std::string>{
            "before suite root",
            "before test passing case",
            "pass test passing case",
            "pass suite"
        });
    });

    it("reports skipped test cases", [] {
        define_root_suite("root", [] {
            it("skipped case", skip("disabled"), [] {});
        });

        auto reporter = recording_reporter();

        assert_true(current_test_registry().run(reporter));
        assert_equal(reporter.skipped_tests.size(), 1uz);
        assert_equal(reporter.skipped_tests.front().reason(), "disabled");
        assert_equal(reporter.events, std::vector<std::string>{
            "before suite root",
            "before test skipped case",
            "skip test skipped case",
            "pass suite"
        });
    });

    it("reports assertion errors", [] {
        define_root_suite("root", [] {
            it("broken case", [] {
                assert_equal(1, 2);
            });
        });

        auto reporter = recording_reporter();

        assert_false(current_test_registry().run(reporter));
        assert_equal(reporter.failures.size(), 1uz);
        assert_equal(reporter.failures.front().description(), "root broken case");
        assert_contain(reporter.failures.front().error_message(), "Expected <1> to equal <2>");
        assert_true(reporter.failures.front().actual().has_value());
        assert_true(reporter.failures.front().expected().has_value());
    });

    it("reports standard exceptions", [] {
        define_root_suite("root", [] {
            it("runtime case", [] {
                throw std::runtime_error("boom");
            });
        });

        auto reporter = recording_reporter();

        assert_false(current_test_registry().run(reporter));
        assert_equal(reporter.failures.size(), 1uz);
        assert_equal(reporter.failures.front().description(), "root runtime case");
        assert_equal(reporter.failures.front().error_message(), "boom");
    });

    it("reports unknown exceptions", [] {
        define_root_suite("root", [] {
            it("unknown case", [] {
                throw 123;
            });
        });

        auto reporter = recording_reporter();

        assert_false(current_test_registry().run(reporter));
        assert_equal(reporter.failures.size(), 1uz);
        assert_equal(reporter.failures.front().description(), "root unknown case");
        assert_equal(reporter.failures.front().error_message(), "Unknown exception");
    });
}
