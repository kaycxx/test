// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the before_all hook registration function.
 */

#include <source_location>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>

namespace kaycxx::test {

/**
 * Registers a hook that runs once before any tests in the current suite are executed.
 *
 * @param body      The hook callback to register.
 * @param location  Source location used when the hook throws an exception without its own location.
 */
void before_all(callback body, std::source_location location = std::source_location::current());

/**
 * Registers a hook that runs once before any tests in the current suite are executed.
 *
 * @param value  The hook to register.
 */
void before_all(hook value);

} // namespace kaycxx::test
