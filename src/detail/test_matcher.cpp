// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_matcher.hpp"

#include <algorithm>
#include <regex>
#include <string>

namespace kaycxx::test::detail {

namespace {

/**
 * Normalizes path separators for platform-independent substring matching.
 *
 * @param path  Path to normalize.
 * @returns Normalized path.
 */
std::string normalize_path(std::string path) {
    std::ranges::replace(path, '\\', '/');
    return path;
}

} // namespace

test_matcher::test_matcher(test_filter const& filter) {
    paths_.reserve(filter.paths.size());
    for (auto const& path : filter.paths) {
        paths_.push_back(normalize_path(path));
    }
    if (filter.name_pattern) {
        name_pattern_.emplace(*filter.name_pattern);
    }
}

bool test_matcher::matches_path(std::source_location const& location) const {
    if (paths_.empty()) {
        return true;
    }

    auto const file_name = normalize_path(location.file_name());
    for (auto const& path : paths_) {
        if (file_name.contains(path)) {
            return true;
        }
    }
    return false;
}

bool test_matcher::matches_name(std::string_view description) const {
    return !name_pattern_ || std::regex_search(description.begin(), description.end(), *name_pattern_);
}

} // namespace kaycxx::test::detail
