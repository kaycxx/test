// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the reporter interface used by the test runner.
 */

#include <cstddef>
#include <string_view>

#include <kaycxx/test/failure.hpp>
#include <kaycxx/test/skipped.hpp>

namespace kaycxx::test {

/**
 * Receives lifecycle events from the test runner.
 */
class reporter {
public:
    /** Destroys the reporter. */
    virtual ~reporter() = default;

    /**
     * Called before the full test run starts.
     *
     * @param num_suites  Number of registered test suites.
     * @param num_tests   Number of registered test cases.
     */
    virtual void before_test_suites(std::size_t num_suites, std::size_t num_tests) = 0;

    /**
     * Called before a test suite starts.
     *
     * @param description  Suite description.
     * @param num_suites   Number of nested suites.
     * @param num_tests    Number of test cases in this suite and its nested suites.
     */
    virtual void before_test_suite(std::string_view description, std::size_t num_suites, std::size_t num_tests) = 0;

    /**
     * Called before a test case starts.
     *
     * This is also called for skipped tests so before/after test case events stay symmetric.
     *
     * @param description  Test case description.
     */
    virtual void before_test_case(std::string_view description) = 0;

    /**
     * Called after a test case passed.
     *
     * @param description  Test case description.
     */
    virtual void after_test_case(std::string_view description) = 0;

    /**
     * Called after a test case was skipped.
     *
     * @param description  Test case description.
     * @param result       Skip result.
     */
    virtual void after_test_case(std::string_view description, skipped result) = 0;

    /**
     * Called after a test case failed.
     *
     * @param description  Test case description.
     * @param result       Failure result.
     */
    virtual void after_test_case(std::string_view description, failure result) = 0;

    /** Called after a test suite passed. */
    virtual void after_test_suite() = 0;

    /**
     * Called after a test suite failed.
     *
     * @param result         Failure result.
     * @param skipped_tests  Number of tests skipped because the suite could not run.
     */
    virtual void after_test_suite(failure result, std::size_t skipped_tests) = 0;

    /** Called after the full test run finished. */
    virtual void after_test_suites() = 0;
};

} // namespace kaycxx::test
