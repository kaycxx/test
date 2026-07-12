// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal test suite node.
 */

#include <memory>
#include <source_location>
#include <string>
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
     */
    explicit test_suite(std::string_view description, std::source_location location);

    /**
     * Creates a nested test suite.
     *
     * @param description  Human-readable suite description.
     * @param location     Source location of the suite declaration.
     * @param parent       Parent suite owning this suite.
     */
    explicit test_suite(std::string_view description, std::source_location location, test_suite& parent);

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

    /**
     * Runs before_each hooks from the root suite to this suite.
     *
     * @param options  Test execution options.
     */
    void run_before_each_hooks(run_options const& options);

    /**
     * Runs after_each hooks from this suite to the root suite.
     *
     * @param options  Test execution options.
     */
    void run_after_each_hooks(run_options const& options);

    /**
     * Executes this suite, including suite hooks and all child nodes in declaration order.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @param matcher   Test filter matcher.
     * @param options   Test execution options.
     * @returns True if the suite passed, false if the suite or any child node failed.
     */
    bool run(reporter& reporter, test_matcher const& matcher, run_options const& options) override;

    /**
     * Adds all test cases below this suite to a test list.
     *
     * @param tests    Test list receiving full test descriptions.
     * @param matcher  Test filter matcher.
     */
    void list_tests(std::vector<std::string>& tests, test_matcher const& matcher) const override;

    /**
     * Counts this suite and all selected descendants.
     *
     * @param matcher  Test filter matcher.
     * @returns Selected suite and test counts.
     */
    test_counts counts(test_matcher const& matcher) const override;

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

};

} // namespace kaycxx::test::detail
