// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/describe.hpp>

#include <utility>

#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void describe(std::string_view description, callback body) {
    active_registry().add_suite(description, std::move(body));
}

} // namespace kaycxx::test
