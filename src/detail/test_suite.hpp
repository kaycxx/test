// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal test suite node.
 */

#include <cstddef>
#include <memory>
#include <optional>
#include <source_location>
#include <string_view>
#include <vector>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/skip.hpp>
#include <kaycxx/test/test_registry.hpp>

#include "test_node.hpp"

namespace kaycxx::test::detail {

class test_matcher;

/**
 * Internal executable node representing a test suite.
 *
 * A suite owns child suites and test cases in declaration order and stores hooks registered for that suite.
 */
class test_suite : public test_node {
public:
    /**
     * Creates a test suite.
     *
     * @param description  Human-readable suite description.
     * @param location     Source location of the suite declaration.
     * @param parent       Optional parent suite.
     */
    explicit test_suite(std::string_view description, std::source_location location, test_suite* parent = nullptr);

    /**
     * Adds a nested suite.
     *
     * @param description  Human-readable suite description.
     * @param location     Source location of the suite declaration.
     * @returns The created nested suite.
     */
    test_suite& add_suite(std::string_view description, std::source_location location);

    /**
     * Adds a test case to this suite.
     *
     * @param description  Human-readable test description.
     * @param body         Test callback to execute.
     * @param location     Source location of the it() call.
     */
    void add_test(std::string_view description, callback body, std::source_location location);

    /**
     * Adds a test case with a skip condition to this suite.
     *
     * @param description     Human-readable test description.
     * @param skip_condition  Condition deciding if the test should be skipped.
     * @param body            Test callback to execute.
     * @param location        Source location of the it() call.
     */
    void add_test(std::string_view description, skip_condition skip_condition, callback body, std::source_location location);

    /**
     * Adds a before_all hook to this suite.
     *
     * @param value  Hook callback to register.
     */
    void add_before_all_hook(kaycxx::test::hook value);

    /**
     * Adds a before_each hook to this suite.
     *
     * @param value  Hook callback to register.
     */
    void add_before_each_hook(kaycxx::test::hook value);

    /**
     * Adds an after_each hook to this suite.
     *
     * @param value  Hook callback to register.
     */
    void add_after_each_hook(kaycxx::test::hook value);

    /**
     * Adds an after_all hook to this suite.
     *
     * @param value  Hook callback to register.
     */
    void add_after_all_hook(kaycxx::test::hook value);

    /** Runs before_each hooks from the root suite to this suite. */
    void run_before_each_hooks();

    /** Runs after_each hooks from this suite to the root suite. */
    void run_after_each_hooks();

    /**
     * Executes this suite, including suite hooks and all child nodes in declaration order.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @returns True if the suite passed, false if the suite or any child node failed.
     */
    bool run(reporter& reporter) override;

    /**
     * Executes selected tests in this suite, including suite hooks and selected child nodes.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @param matcher   Test filter matcher.
     * @returns True if the selected tests passed or no tests are selected, false otherwise.
     */
    bool run(reporter& reporter, test_matcher const& matcher);

    /**
     * Executes one registered test case by numeric id.
     *
     * @param target_id   Numeric test id to run.
     * @param current_id  Current traversal id.
     * @param reporter    Reporter receiving lifecycle events.
     * @returns True if the selected test passed or is not part of this suite, false if it failed.
     */
    bool run_test(std::size_t target_id, std::size_t& current_id, reporter& reporter);

    /**
     * Adds all test cases below this suite to a test list.
     *
     * @param tests    Test list receiving test cases.
     * @param next_id  Next numeric test id.
     */
    void list_tests(std::vector<test_info>& tests, std::size_t& next_id) const;

    /**
     * Adds selected test cases below this suite to a test list.
     *
     * @param tests    Test list receiving selected test cases.
     * @param next_id  Next numeric test id in the unfiltered tree.
     * @param matcher  Test filter matcher.
     */
    void list_tests(std::vector<test_info>& tests, std::size_t& next_id, test_matcher const& matcher) const;

    /**
     * @returns The number of registered test suites.
     */
    std::size_t num_test_suites() const;

    /**
     * Returns the number of nested suites containing selected tests.
     *
     * @param matcher  Test filter matcher.
     * @returns The number of selected nested suites.
     */
    std::size_t num_test_suites(test_matcher const& matcher) const;

    /**
     * @returns The number of registered test cases.
     */
    std::size_t num_test_cases() const;

    /**
     * Returns the number of selected test cases below this suite.
     *
     * @param matcher  Test filter matcher.
     * @returns The number of selected test cases.
     */
    std::size_t num_test_cases(test_matcher const& matcher) const;

private:
    /** Child suites and test cases in declaration order. */
    std::vector<std::unique_ptr<test_node>> children_;

    /** Hooks executed before this suite's child nodes run. */
    std::vector<kaycxx::test::hook> before_all_hooks_;

    /** Hooks executed before each test in this suite and nested suites. */
    std::vector<kaycxx::test::hook> before_each_hooks_;

    /** Hooks executed after each test in this suite and nested suites. */
    std::vector<kaycxx::test::hook> after_each_hooks_;

    /** Hooks executed after this suite's child nodes run. */
    std::vector<kaycxx::test::hook> after_all_hooks_;

    /**
     * Returns the number of nested suites on the path to a selected test.
     *
     * @param target_id   Numeric test id to find.
     * @param current_id  Current traversal id.
     * @returns Number of nested suites on the selected path, or no value when the test is not below this suite.
     */
    std::optional<std::size_t> selected_suite_count(std::size_t target_id, std::size_t& current_id) const;

    /**
     * Executes either the complete suite or its selected tests.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @param matcher   Optional test filter matcher, or null to execute all tests.
     * @returns True if the executed tests passed, false otherwise.
     */
    bool run(reporter& reporter, test_matcher const* matcher);

};

} // namespace kaycxx::test::detail
