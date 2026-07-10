// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the before_each hook registration function.
 */

#include <source_location>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>

namespace kaycxx::test {

/**
 * Registers a hook that runs before each test in the current suite and its child suites.
 *
 * Hooks run from the outermost suite to the innermost suite.
 *
 * @param body      The hook callback to register.
 * @param location  Source location used when the hook throws an exception without its own location.
 */
void before_each(callback body, std::source_location location = std::source_location::current());

/**
 * Registers a hook that runs before each test in the current suite and its child suites.
 *
 * @param value  The hook to register.
 */
void before_each(hook value);

} // namespace kaycxx::test
