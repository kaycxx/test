// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines internal test filter matching.
 */

#include <optional>
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
     * @throws std::regex_error  When the test name pattern is not a valid regular expression.
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
     * Checks whether a full test description matches the configured name pattern.
     *
     * @param description  Full test description to check.
     * @returns True when no name pattern is configured or the pattern matches.
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

    /** Compiled full-description pattern. */
    std::optional<std::regex> name_pattern_;
};

} // namespace kaycxx::test::detail
