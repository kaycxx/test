// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the test failure result type.
 */

#include <cstdint>
#include <exception>
#include <optional>
#include <source_location>
#include <string>
#include <string_view>

#include <kaycxx/assert.hpp>

namespace kaycxx::test {

/**
 * Failure result passed to reporters when a test case or suite fails.
 */
class failure {
public:
    /**
     * Creates a failure from an assertion error.
     *
     * @param description  Full test or suite description.
     * @param error        Assertion error thrown by kaycxx::assert.
     * @param location     Fallback source location to report for the failure when error has no location.
     */
    explicit failure(std::string_view description, kaycxx::assert::assertion_error const& error, std::source_location location);

    /**
     * Creates a failure from a standard exception and explicit source location.
     *
     * @param description  Full test or suite description.
     * @param error        Exception thrown by the test, suite, or hook.
     * @param location     Source location to report for the failure.
     */
    explicit failure(std::string_view description, std::exception const& error, std::source_location location);

    /**
     * Creates a failure for a non-standard unknown exception.
     *
     * @param description  Full test or suite description.
     * @param location     Source location to report for the failure.
     */
    explicit failure(std::string_view description, std::source_location location);

    /**
     * Returns the full test or suite description.
     *
     * @returns The full test or suite description.
     */
    std::string const& description() const noexcept;

    /**
     * Returns the failure message.
     *
     * @returns The failure message.
     */
    std::string const& error_message() const noexcept;

    /**
     * Returns the source location.
     *
     * @returns The source location.
     */
    std::source_location const& location() const noexcept;

    /**
     * Returns the actual assertion value when available.
     *
     * @returns The actual value string, or an empty optional when the failure did not come from a value assertion.
     */
    std::optional<std::string> const& actual() const noexcept;

    /**
     * Returns the expected assertion value when available.
     *
     * @returns The expected value string, or an empty optional when the failure did not come from a value assertion.
     */
    std::optional<std::string> const& expected() const noexcept;

private:
    /** Full test or suite description. */
    std::string description_;

    /** Failure message. */
    std::string error_message_;

    /** Source location. */
    std::source_location location_;

    /** Actual assertion value when available. */
    std::optional<std::string> actual_;

    /** Expected assertion value when available. */
    std::optional<std::string> expected_;
};

} // namespace kaycxx::test
