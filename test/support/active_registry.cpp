// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "active_registry.hpp"

#include <memory>

namespace kaycxx::test::support {

namespace {

std::unique_ptr<test_registry> registry;

} // namespace

void create_active_registry() {
    registry = std::make_unique<test_registry>();
    set_active_registry(*registry);
}

void destroy_active_registry() {
    reset_active_registry();
    registry.reset();
}

test_registry& current_test_registry() {
    return *registry;
}

} // namespace kaycxx::test::support
