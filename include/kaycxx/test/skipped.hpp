// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the skipped test result type.
 */

#include <string>
#include <string_view>

namespace kaycxx::test {

/**
 * Result object passed to reporters when a test case was skipped.
 */
class skipped {
public:
    /**
     * Creates a skipped test result.
     *
     * @param reason  Optional reason why the test was skipped.
     */
    explicit skipped(std::string_view reason = "");

    /**
     * Returns the skip reason.
     *
     * @returns The skip reason, or an empty string if no reason was provided.
     */
    std::string const& reason() const noexcept;

private:
    /** Reason shown for the skipped test. */
    std::string reason_;
};

} // namespace kaycxx::test
