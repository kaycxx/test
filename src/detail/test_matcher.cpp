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

    name_patterns_.reserve(filter.name_patterns.size());
    for (auto const& pattern : filter.name_patterns) {
        name_patterns_.emplace_back(pattern);
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
    if (name_patterns_.empty()) {
        return true;
    }

    for (auto const& pattern : name_patterns_) {
        if (std::regex_search(description.begin(), description.end(), pattern)) {
            return true;
        }
    }
    return false;
}

bool test_matcher::has_filters() const noexcept {
    return !paths_.empty() || !name_patterns_.empty();
}

} // namespace kaycxx::test::detail
