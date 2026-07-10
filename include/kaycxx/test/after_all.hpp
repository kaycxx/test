// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the after_all hook registration function.
 */

#include <source_location>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>

namespace kaycxx::test {

/**
 * Registers a hook that runs once after all tests in the current suite have finished.
 *
 * @param body      The hook callback to register.
 * @param location  Source location used when the hook throws an exception without its own location.
 */
void after_all(callback body, std::source_location location = std::source_location::current());

/**
 * Registers a hook that runs once after all tests in the current suite have finished.
 *
 * @param value  The hook to register.
 */
void after_all(hook value);

} // namespace kaycxx::test
