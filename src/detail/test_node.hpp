// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal base type for executable test tree nodes.
 */

#include <cstddef>
#include <functional>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#include <kaycxx/test/reporter.hpp>
#include <kaycxx/test/run_options.hpp>

namespace kaycxx::test::detail {

class test_suite;
class test_matcher;

/** Number of selected suites and test cases in a test tree. */
struct test_counts {
    /** Number of selected suites. */
    std::size_t suites;

    /** Number of selected test cases. */
    std::size_t tests;
};

/**
 * Internal base class for nodes that can be executed by the test runner.
 */
class test_node {
public:
    /**
     * Creates a test node.
     *
     * @param description  Human-readable description.
     * @param location     Source location of the node declaration.
     */
    explicit test_node(std::string_view description, std::source_location location);

    /**
     * Creates a nested test node.
     *
     * @param description  Human-readable description.
     * @param location     Source location of the node declaration.
     * @param parent       Suite owning the test node.
     */
    explicit test_node(std::string_view description, std::source_location location, test_suite& parent);

    /** Destroys the test node. */
    virtual ~test_node() = default;

    /**
     * Executes selected tests below this node.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @param matcher   Test filter matcher.
     * @param options   Test execution options.
     * @returns True if the node finished successfully, false if it recorded a failure.
     */
    virtual bool run(reporter& reporter, test_matcher const& matcher, run_options const& options) = 0;

    /**
     * Adds selected test cases below this node to a test list.
     *
     * @param tests    Test list receiving full test descriptions.
     * @param matcher  Test filter matcher.
     */
    virtual void list_tests(std::vector<std::string>& tests, test_matcher const& matcher) const = 0;

    /**
     * Counts selected suites and test cases below this node.
     *
     * @param matcher  Test filter matcher.
     * @returns Selected suite and test counts.
     */
    virtual test_counts counts(test_matcher const& matcher) const = 0;

    /**
     * Returns the description.
     *
     * @returns The description.
     */
    const std::string& description() const;

    /**
     * Returns the node description with prefixed full parent description.
     *
     * @returns The node description with prefixed full parent description.
     */
    std::string full_description() const;

    /**
     * Checks whether this node or one of its parent suites matches a source path filter.
     *
     * @param matcher  Test filter matcher.
     * @returns True when the node hierarchy matches the path filter.
     */
    bool matches_path(test_matcher const& matcher) const;

protected:
    /**
     * Returns the parent suite.
     *
     * @returns The parent suite.
     */
    test_suite& parent() const;

    /** Human-readable node description. */
    std::string description_;

    /** Source location of the node declaration. */
    std::source_location location_;

    /** Parent suite owning this node, or no value for top-level suites. */
    std::optional<std::reference_wrapper<test_suite>> parent_;
};

} // namespace kaycxx::test::detail
