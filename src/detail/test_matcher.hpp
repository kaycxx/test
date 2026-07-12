// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines internal test filter matching.
 */

#include <regex>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#include <kaycxx/test/test_filter.hpp>

namespace kaycxx::test::detail {

/** Compiles and applies a test selection filter. */
class test_matcher {
public:
    /**
     * Creates a matcher for a test filter.
     *
     * @param filter  Test selection filter.
     *
     * @throws std::regex_error  When a test name pattern is not a valid regular expression.
     */
    explicit test_matcher(test_filter const& filter);

    /**
     * Checks whether a source location matches one of the configured path filters.
     *
     * @param location  Source location to check.
     * @returns True when no path filters are configured or at least one path filter matches.
     */
    bool matches_path(std::source_location const& location) const;

    /**
     * Checks whether a full test description matches one of the configured name patterns.
     *
     * @param description  Full test description to check.
     * @returns True when no name patterns are configured or at least one pattern matches.
     */
    bool matches_name(std::string_view description) const;

    /**
     * Checks whether any path or name filters are configured.
     *
     * @returns True when the matcher restricts the selected tests.
     */
    bool has_filters() const noexcept;

private:
    /** Normalized source path substrings. */
    std::vector<std::string> paths_;

    /** Compiled full-description patterns. */
    std::vector<std::regex> name_patterns_;
};

} // namespace kaycxx::test::detail
