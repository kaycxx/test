// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_node.hpp"

#include <format>

#include "test_matcher.hpp"
#include "test_suite.hpp"

namespace kaycxx::test::detail {

test_node::test_node(std::string_view description, std::source_location location, test_suite* parent)
    : description_(description),
      location_(location),
      parent_(parent)
{}

const std::string& test_node::description() const {
    return description_;
}

std::string test_node::full_description() const {
    if (!parent_) {
        return description_;
    }
    auto parent_description = parent_->full_description();
    if (parent_description == "") {
        return description_;
    }
    return std::format("{} {}", parent_description, description_);
}

bool test_node::matches_path(test_matcher const& matcher) const {
    return matcher.matches_path(location_) || (parent_ != nullptr && parent_->matches_path(matcher));
}

} // namespace kaycxx::test::detail
