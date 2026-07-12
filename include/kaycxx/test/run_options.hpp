// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines test execution options.
 */

namespace kaycxx::test {

/** Configures the execution of registered tests. */
struct run_options {
    /** Allow unexpected exceptions to escape the test framework so a debugger can stop at the failure. */
    bool break_on_exception = false;
};

} // namespace kaycxx::test
