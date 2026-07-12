// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal test case node.
 */

#include <cstddef>
#include <optional>
#include <source_location>
#include <string_view>
#include <vector>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/skip.hpp>
#include <kaycxx/test/test_registry.hpp>

#include "test_node.hpp"

namespace kaycxx::test::detail {

class test_matcher;
class test_suite;

/**
 * Internal executable node representing a single registered test case.
 */
class test_case : public test_node {
public:
    /**
     * Creates a test case.
     *
     * @param description     Human-readable test description.
     * @param body            Test callback to execute.
     * @param location        Source location of the it() call.
     * @param parent          Suite owning the test case.
     * @param skip_condition  Optional condition deciding if the test should be skipped.
     */
    test_case(
        std::string_view description,
        callback body,
        std::source_location location,
        test_suite* parent,
        std::optional<skip_condition> skip_condition = std::nullopt
    );

    /**
     * Executes the test case with the parent suite's before_each and after_each hooks.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @returns True if the test case passed, false if it failed.
     */
    bool run(reporter& reporter) override;

    /**
     * Checks whether this test case is selected by a matcher.
     *
     * @param matcher  Test filter matcher.
     * @returns True when source path and full-description filters match.
     */
    bool matches(test_matcher const& matcher) const;

    /**
     * Adds this test case to a test list.
     *
     * @param tests    Test list receiving the test case.
     * @param next_id  Next numeric test id.
     */
    void list_tests(std::vector<test_info>& tests, std::size_t& next_id) const;

private:
    /** Test callback to execute. */
    callback body_;

    /** Optional skip condition checked before before_each hooks and test code run. */
    std::optional<skip_condition> skip_condition_;
};

} // namespace kaycxx::test::detail
