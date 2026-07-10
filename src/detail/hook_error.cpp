// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "hook_error.hpp"

#include <string>

namespace kaycxx::test::detail {

hook_error::hook_error(
    std::string const& message,
    std::source_location location
) : std::logic_error(message),
    location_(location)
{}

std::source_location const& hook_error::location() const noexcept {
    return location_;
}

} // namespace kaycxx::test::detail
