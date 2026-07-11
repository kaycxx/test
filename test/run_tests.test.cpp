// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <sstream>
#include <stdexcept>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/active_registry.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;
using namespace kaycxx::test::support;

using kaycxx::test::detail::define_root_suite;

suite("run_tests") {
    before_each([] {
        create_active_registry();
    });

    after_each([] {
        destroy_active_registry();
    });

    after_all([] {
        destroy_active_registry();
    });

    it("returns zero for passing registry", [] {
        define_root_suite("math", [] {
            it("passes", [] {});
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::none), 0);
        assert_contain(output.str(), "1 tests, 1 passing");
        assert_not_contain(output.str(), "\033[");
    });

    it("emits ansi sequences when colors are forced", [] {
        define_root_suite("colors", [] {
            it("passes", [] {});
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::formatting_only), 0);
        auto const text = output.str();
        assert_contain(text, "\033[1mcolors\033[22m");
        assert_contain(text, "\033[32m\033[1m");
        assert_contain(text, "\033[90m(");
        assert_contain(text, "\033[32m1 passing\033[0m");
    });

    it("returns one for failing test case", [] {
        define_root_suite("broken", [] {
            it("fails", [] {
                throw std::runtime_error("boom");
            });
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::none), 1);
        auto const text = output.str();
        assert_contain(text, "1 tests, 0 passing");
        assert_contain(text, "1 failing");
        assert_contain(text, "boom");
    });

    it("runs one selected test", [] {
        auto first_ran = false;
        auto second_ran = false;

        define_root_suite("selected", [&] {
            it("first", [&] {
                first_ran = true;
            });
            it("second", [&] {
                second_ran = true;
            });
        });

        auto output = std::ostringstream();

        assert_equal(run_test(current_test_registry(), "2", output, kaycxx::term::ansi_mode::none), 0);
        assert_false(first_ran);
        assert_true(second_ran);
        auto const text = output.str();
        assert_contain(text, "1 tests, 1 passing");
        assert_contain(text, "second");
        assert_not_contain(text, "first");
    });

    it("skips test before hooks and body run", [] {
        auto before_each_ran = false;
        auto body_ran = false;

        define_root_suite("skip suite", [&] {
            before_each([&] {
                before_each_ran = true;
            });
            it("skipped test", skip("disabled"), [&] {
                body_ran = true;
            });
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::none), 0);
        assert_false(before_each_ran);
        assert_false(body_ran);
        auto const text = output.str();
        assert_contain(text, "(Skipped: disabled)");
        assert_contain(text, "1 skipped");
    });

    it("skips suite children when before_all fails", [] {
        auto body_ran = false;

        define_root_suite("setup", [&] {
            before_all([] {
                throw std::runtime_error("setup failed");
            });
            it("direct child", [&] {
                body_ran = true;
            });
            describe("nested", [&] {
                it("nested child", [&] {
                    body_ran = true;
                });
            });
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::none), 1);
        assert_false(body_ran);
        auto const text = output.str();
        assert_contain(text, "2 tests, 0 passing");
        assert_contain(text, "2 skipped");
        assert_contain(text, "1 suite failure");
        assert_contain(text, "setup failed");
    });

    it("reports after_all failures without marking tests as skipped", [] {
        auto body_ran = false;

        define_root_suite("teardown", [&] {
            it("child", [&] {
                body_ran = true;
            });
            after_all([] {
                throw std::runtime_error("teardown failed");
            });
        });

        auto output = std::ostringstream();

        assert_equal(run_tests(current_test_registry(), output, kaycxx::term::ansi_mode::none), 1);
        assert_true(body_ran);
        auto const text = output.str();
        assert_contain(text, "1 tests, 1 passing");
        assert_contain(text, "1 suite failure");
        assert_contain(text, "teardown failed");
    });
}
