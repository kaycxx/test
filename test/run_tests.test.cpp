// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <array>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

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

    it("writes CTest entries for names containing\nnewlines; regex .^$|()[]{}*+?\\ and brackets ]] ]=] ]==]", [] {
        auto const file_name = (std::filesystem::temp_directory_path() / "kaycxx-test-write-ctest.test.cmake").string();
        auto executable = std::string("test executable ]] ]=] ]==]");
        auto action = std::string("--write-ctest");
        auto output_file = file_name;
        auto arguments = std::array<char*, 3>({ executable.data(), action.data(), output_file.data() });

        assert_equal(run_tests(static_cast<int>(arguments.size()), arguments.data()), 0);

        auto input = std::ifstream(file_name);
        assert_true(input.good());
        auto content = std::ostringstream();
        content << input.rdbuf();
        assert_contain(content.str(), R"EXPECTED(add_test([===[run_tests writes CTest entries for names containing
newlines; regex .^$|()[]{}*+?\ and brackets ]] ]=] ]==]]===] [===[test executable ]] ]=] ]==]]===] --test-name-pattern [[^run_tests writes CTest entries for names containing
newlines; regex \.\^\$\|\(\)\[\]\{\}\*\+\?\\ and brackets \]\] \]=\] \]==\]$]])
)EXPECTED");

        input.close();
        std::filesystem::remove(file_name);
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
