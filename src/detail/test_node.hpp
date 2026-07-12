// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal base type for executable test tree nodes.
 */

#include <source_location>
#include <string>
#include <string_view>

#include <kaycxx/test/reporter.hpp>

namespace kaycxx::test::detail {

class test_suite;
class test_matcher;

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
     * @param parent       Suite owning the test node.
     */
    explicit test_node(std::string_view description, std::source_location location, test_suite* parent);

    /** Destroys the test node. */
    virtual ~test_node() = default;

    /**
     * Executes the node.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @returns True if the node finished successfully, false if it recorded a failure.
     */
    virtual bool run(reporter& reporter) = 0;

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
    /** Human-readable node description. */
    std::string description_;

    /** Source location of the node declaration. */
    std::source_location location_;

    /** Parent suite owning this node, or null for top-level suites. */
    test_suite* parent_;
};

} // namespace kaycxx::test::detail
