// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines skip conditions for test cases.
 */

#include <functional>
#include <string>
#include <string_view>

namespace kaycxx::test {

/**
 * Condition used by a test case to decide whether it should be skipped before running hooks and test code.
 */
class skip_condition {
public:
    /**
     * Creates a condition that always skips the test.
     *
     * @param reason  Optional reason why the test should be skipped.
     */
    explicit skip_condition(std::string_view reason = "");

    /**
     * Creates a dynamic skip condition.
     *
     * @param condition  Callback returning true when the test should be skipped.
     * @param reason     Optional reason why the test should be skipped.
     */
    skip_condition(std::move_only_function<bool()> condition, std::string_view reason = "");

    /**
     * Checks if the test should be skipped.
     *
     * @returns True if the test should be skipped, false if it should run.
     */
    bool should_skip();

    /**
     * Returns the skip reason.
     *
     * @returns The skip reason, or an empty string if no reason was provided.
     */
    std::string const& reason() const noexcept;

private:
    /** Optional dynamic condition returning true when the test should be skipped. */
    std::move_only_function<bool()> condition_;

    /** Reason shown when the test is skipped. */
    std::string reason_;
};

/**
 * Creates a condition that always skips the test.
 *
 * @param reason  Optional reason why the test should be skipped.
 * @returns The skip condition.
 */
skip_condition skip(std::string_view reason = "");

/**
 * Creates a condition that skips the test when the callback returns true.
 *
 * @param condition  Callback returning true when the test should be skipped.
 * @param reason     Optional reason why the test should be skipped.
 * @returns The skip condition.
 */
skip_condition skip_if(std::move_only_function<bool()> condition, std::string_view reason = "");

/**
 * Creates a condition that skips the test unless the callback returns true.
 *
 * @param condition  Callback returning true when the test should run.
 * @param reason     Optional reason why the test should be skipped.
 * @returns The skip condition.
 */
skip_condition skip_unless(std::move_only_function<bool()> condition, std::string_view reason = "");

} // namespace kaycxx::test
