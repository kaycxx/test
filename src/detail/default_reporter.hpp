// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the default terminal reporter.
 */

#include <chrono>
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string_view>
#include <vector>

#include <kaycxx/term.hpp>
#include <kaycxx/test/failure.hpp>
#include <kaycxx/test/reporter.hpp>
#include <kaycxx/test/skipped.hpp>

namespace kaycxx::test::detail {

/**
 * Default reporter that writes a human-readable test report to a terminal stream.
 */
class default_reporter : public reporter {
public:
    /**
     * Creates a default reporter.
     *
     * @param output     Output stream receiving the report.
     * @param ansi_mode  Controls ANSI/VT control sequence output.
     */
    explicit default_reporter(
        std::ostream& output = std::cout,
        kaycxx::term::ansi_mode ansi_mode = kaycxx::term::ansi_mode::automatic
    );

    /**
     * Starts the test run.
     *
     * @param num_test_suites  Number of registered test suites.
     * @param num_test_cases   Number of registered test cases.
     */
    void before_test_suites(std::size_t num_test_suites, std::size_t num_test_cases) override;

    /**
     * Starts a test suite.
     *
     * @param description      Suite description.
     * @param num_test_suites  Number of nested suites.
     * @param num_test_cases   Number of test cases in this suite and its nested suites.
     */
    void before_test_suite(std::string_view description, std::size_t num_test_suites, std::size_t num_test_cases) override;

    /**
     * Starts a test case.
     *
     * @param description  Test case description.
     */
    void before_test_case(std::string_view description) override;

    /**
     * Reports a passed test case.
     *
     * @param description  Test case description.
     */
    void after_test_case(std::string_view description) override;

    /**
     * Reports a skipped test case.
     *
     * @param description  Test case description.
     * @param result       Skip result.
     */
    void after_test_case(std::string_view description, skipped result) override;

    /**
     * Reports a failed test case.
     *
     * @param description  Test case description.
     * @param result       Failure result.
     */
    void after_test_case(std::string_view description, failure result) override;

    /** Reports a passed test suite. */
    void after_test_suite() override;

    /**
     * Reports a failed test suite.
     *
     * @param result         Failure result.
     * @param skipped_tests  Number of tests skipped because the suite could not run.
     */
    void after_test_suite(failure result, std::size_t skipped_tests) override;

    /** Finishes the test run and prints the summary. */
    void after_test_suites() override;

private:
    /** Terminal wrapper used for styled output. */
    kaycxx::term::terminal term_;

    /** Current indentation level in spaces. */
    std::size_t level_ = 2;

    /** Total number of registered test cases. */
    std::size_t num_test_cases_ = 0;

    /** Number of failed test cases. */
    std::size_t num_test_failures_ = 0;

    /** Number of failed test suites. */
    std::size_t num_suite_failures_ = 0;

    /** Number of skipped test cases. */
    std::size_t num_skipped_tests_ = 0;

    /** Recorded failures used for detailed failure output. */
    std::vector<failure> failures_;

    /** Start time of the full test run. */
    std::chrono::steady_clock::time_point start_time_;

    /** Start time of the currently running test case. */
    std::chrono::steady_clock::time_point test_start_time_;
};

} // namespace kaycxx::test::detail
