// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test::support {

void create_active_registry();

void destroy_active_registry();

test_registry& current_test_registry();

} // namespace kaycxx::test::support
