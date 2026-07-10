// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/it.hpp>

#include <utility>

#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void it(std::string_view description, callback body, std::source_location location) {
    active_registry().add_test(description, std::move(body), location);
}

void it(std::string_view description, skip_condition skip_condition, callback body, std::source_location location) {
    active_registry().add_test(description, std::move(skip_condition), std::move(body), location);
}

} // namespace kaycxx::test
