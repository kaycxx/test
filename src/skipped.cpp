// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/skipped.hpp>

namespace kaycxx::test {

skipped::skipped(std::string_view reason)
    : reason_(reason)
{}

std::string const& skipped::reason() const noexcept {
    return reason_;
}

} // namespace kaycxx::test
