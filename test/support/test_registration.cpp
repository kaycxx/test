// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_registration.hpp"

#include <utility>

namespace kaycxx::test::support {

void register_test_from_support(test_registry& registry, std::string_view description, callback body) {
    registry.add_test(description, std::move(body));
}

} // namespace kaycxx::test::support
