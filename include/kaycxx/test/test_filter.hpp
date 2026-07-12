// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines test selection filters.
 */

#include <optional>
#include <string>
#include <vector>

namespace kaycxx::test {

/**
 * Selects registered tests by source path and full description.
 *
 * Multiple path filters are combined with OR. Path and name filters are combined with AND. Empty filters select all registered tests.
 */
struct test_filter {
    /** Source path substrings to match against tests and their parent suites. */
    std::vector<std::string> paths;

    /** Optional regular expression matched against full test descriptions. */
    std::optional<std::string> name_pattern;
};

} // namespace kaycxx::test
