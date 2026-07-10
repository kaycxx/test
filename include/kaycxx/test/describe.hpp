// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the describe function for nested test suites.
 */

#include <string_view>

#include <kaycxx/test/callback.hpp>

namespace kaycxx::test {

/**
 * Defines a nested test suite and immediately executes its registration body.
 *
 * Tests and hooks registered inside the body are added to the nested suite.
 *
 * @param description  Human-readable suite description.
 * @param body         Registration callback that defines child suites, tests, and hooks.
 */
void describe(std::string_view description, callback body);

} // namespace kaycxx::test
