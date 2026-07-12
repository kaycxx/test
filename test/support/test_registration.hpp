// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

#include <string_view>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test::support {

/**
 * Registers a test whose own source location points to the support implementation.
 *
 * @param registry     Registry receiving the test.
 * @param description  Human-readable test description.
 * @param body         Test callback to execute.
 */
void register_test_from_support(test_registry& registry, std::string_view description, callback body);

} // namespace kaycxx::test::support
