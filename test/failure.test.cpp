// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <source_location>
#include <stdexcept>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

using namespace kaycxx::assert;
using namespace kaycxx::test;

suite("failure") {
    it("copies assertion failure details", [] {
        try {
            assert_equal(1, 2);
        } catch (assertion_error const& error) {
            auto const location = std::source_location::current();
            auto result = failure("math fails", error, location);

            assert_equal(result.description(), "math fails");
            assert_contain(result.error_message(), "Expected <1> to equal <2>");
            assert_not_equal(result.location().file_name(), "");
            assert_not_equal(result.location().function_name(), "");
            assert_greater(result.location().line(), 0u);
            assert_greater(result.location().column(), 0u);
            assert_true(result.actual().has_value());
            assert_true(result.expected().has_value());
            assert_equal(result.actual().value(), "1");
            assert_equal(result.expected().value(), "2");
            return;
        }

        assert_true(false, "assert_equal should have thrown");
    });

    it("stores standard exception details with explicit location", [] {
        auto const location = std::source_location::current();
        auto result = failure("runtime fails", std::runtime_error("boom"), location);

        assert_equal(result.description(), "runtime fails");
        assert_equal(result.error_message(), "boom");
        assert_equal(result.location().file_name(), location.file_name());
        assert_equal(result.location().function_name(), location.function_name());
        assert_equal(result.location().line(), location.line());
        assert_equal(result.location().column(), location.column());
        assert_false(result.actual().has_value());
        assert_false(result.expected().has_value());
    });

    it("stores unknown exception details with explicit location", [] {
        auto const location = std::source_location::current();
        auto result = failure("unknown fails", location);

        assert_equal(result.description(), "unknown fails");
        assert_equal(result.error_message(), "Unknown exception");
        assert_equal(result.location().file_name(), location.file_name());
        assert_equal(result.location().function_name(), location.function_name());
        assert_equal(result.location().line(), location.line());
        assert_equal(result.location().column(), location.column());
        assert_false(result.actual().has_value());
        assert_false(result.expected().has_value());
    });
}
