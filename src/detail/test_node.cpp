// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_node.hpp"

#include <format>

#include "test_suite.hpp"

namespace kaycxx::test::detail {

test_node::test_node(std::string_view description, test_suite* parent)
    : description_(description),
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

} // namespace kaycxx::test::detail
