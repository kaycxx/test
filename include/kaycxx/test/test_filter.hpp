// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines test selection filters.
 */

#include <string>
#include <vector>

namespace kaycxx::test {

/**
 * Selects registered tests by source path and full description.
 *
 * Multiple path filters and multiple name patterns are each combined with OR. Path and name filters are combined with AND. Empty filters select all registered
 * tests.
 */
struct test_filter {
    /** Source path substrings to match against tests and their parent suites. */
    std::vector<std::string> paths;

    /** Regular expressions matched against full test descriptions. */
    std::vector<std::string> name_patterns;
};

} // namespace kaycxx::test
