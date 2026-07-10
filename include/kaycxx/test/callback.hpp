// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the callback type used by the test framework.
 */

#include <functional>

namespace kaycxx::test {

/**
 * Callable block used by tests, suites, and hooks.
 *
 * The callback is move-only so test code can capture move-only resources.
 */
using callback = std::move_only_function<void()>;

} // namespace kaycxx::test
