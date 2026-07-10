// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the it function for registering test cases.
 */

#include <source_location>
#include <string_view>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/skip.hpp>

namespace kaycxx::test {

/**
 * Registers a test case in the current suite.
 *
 * @param description  Human-readable test description.
 * @param body         Test callback to execute when the test case runs.
 * @param location     Source location of the test registration.
 */
void it(std::string_view description, callback body, std::source_location location = std::source_location::current());

/**
 * Registers a test case in the current suite with a skip condition.
 *
 * The skip condition is checked before before_each hooks and test code are executed.
 *
 * @param description     Human-readable test description.
 * @param skip_condition  Condition deciding if the test should be skipped.
 * @param body            Test callback to execute when the test case runs.
 * @param location        Source location of the test registration.
 */
void it(std::string_view description, skip_condition skip_condition, callback body, std::source_location location = std::source_location::current());

} // namespace kaycxx::test
