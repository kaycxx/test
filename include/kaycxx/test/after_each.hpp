// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the after_each hook registration function.
 */

#include <source_location>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>

namespace kaycxx::test {

/**
 * Registers a hook that runs after each test in the current suite and its child suites.
 *
 * Hooks run from the innermost suite to the outermost suite.
 *
 * @param body      The hook callback to register.
 * @param location  Source location used when the hook throws an exception without its own location.
 */
void after_each(callback body, std::source_location location = std::source_location::current());

/**
 * Registers a hook that runs after each test in the current suite and its child suites.
 *
 * @param value  The hook to register.
 */
void after_each(hook value);

} // namespace kaycxx::test
