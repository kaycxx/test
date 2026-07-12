// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines functions for running registered tests.
 */

#include <iosfwd>

#include <kaycxx/term/ansi_mode.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

/**
 * Runs the global test registry and writes the report to standard output.
 *
 * @returns Process exit code 0 when all tests pass, otherwise 1.
 */
int run_tests();

/**
 * Runs the global test registry according to command-line arguments.
 *
 * Positional path filters select tests by source file or directory. The -t and --test-name-pattern options select full descriptions with a regular expression.
 * Supported actions are --help and --write-ctest.
 *
 * @param argc  Number of command-line arguments.
 * @param argv  Command-line argument values.
 * @returns Process exit code 0 when the command succeeds, otherwise non-zero.
 */
int run_tests(int argc, char* argv[]);

/**
 * Runs the global test registry and writes the report to the given stream.
 *
 * @param output  Output stream receiving the test report.
 * @returns Process exit code 0 when all tests pass, otherwise 1.
 */
int run_tests(std::ostream& output);

/**
 * Runs the global test registry and writes the report to the given stream.
 *
 * @param output     Output stream receiving the test report.
 * @param ansi_mode  Controls ANSI/VT control sequence output.
 * @returns Process exit code 0 when all tests pass, otherwise 1.
 */
int run_tests(std::ostream& output, kaycxx::term::ansi_mode ansi_mode);

/**
 * Runs the given test registry and writes the report to the given stream.
 *
 * @param registry  Registry containing the suites to run.
 * @param output    Output stream receiving the test report.
 * @returns Process exit code 0 when all tests pass, otherwise 1.
 */
int run_tests(test_registry& registry, std::ostream& output);

/**
 * Runs the given test registry and writes the report to the given stream.
 *
 * @param registry   Registry containing the suites to run.
 * @param output     Output stream receiving the test report.
 * @param ansi_mode  Controls ANSI/VT control sequence output.
 * @returns Process exit code 0 when all tests pass, otherwise 1.
 */
int run_tests(test_registry& registry, std::ostream& output, kaycxx::term::ansi_mode ansi_mode);

} // namespace kaycxx::test
