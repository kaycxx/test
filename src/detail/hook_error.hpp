// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the internal hook error type.
 */

#include <source_location>
#include <stdexcept>
#include <string>

namespace kaycxx::test::detail {

/**
 * Internal exception thrown when a hook fails.
 */
class hook_error : public std::logic_error {
public:
    /**
     * Creates a hook error.
     *
     * @param message   Error message.
     * @param location  Source location of the hook registration.
     */
    explicit hook_error(
        std::string const& message,
        std::source_location location
    );

    /**
     * Returns the source location of the failed hook.
     *
     * @returns The source location of the failed hook.
     */
    std::source_location const& location() const noexcept;

private:
    /** Source location of the hook registration. */
    std::source_location location_;
};

} // namespace kaycxx::test::detail
