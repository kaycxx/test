// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines the suite macro for top-level test suite registration.
 */

#include <string_view>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/describe.hpp>

/** @cond internal */
namespace kaycxx::test::detail {

/**
 * Defines a top-level suite from the suite macro.
 *
 * @param description  Human-readable suite description.
 * @param body         Registration callback that defines child suites, tests, and hooks.
 */
void define_root_suite(std::string_view description, callback body);

} // namespace kaycxx::test::detail
/** @endcond */

/** @cond internal */
#define KAYCXX_TEST_CONCAT2(a, b) a##b
#define KAYCXX_TEST_CONCAT(a, b) KAYCXX_TEST_CONCAT2(a, b)
#define KAYCXX_TEST_SUITE(description, id) \
    static void KAYCXX_TEST_CONCAT(_kaycxx_test_suite_body_, id)(); \
    [[maybe_unused]] static const bool KAYCXX_TEST_CONCAT(_kaycxx_test_suite_, id) = [] { \
        ::kaycxx::test::detail::define_root_suite(description, KAYCXX_TEST_CONCAT(_kaycxx_test_suite_body_, id)); \
        return true; \
    }(); \
    static void KAYCXX_TEST_CONCAT(_kaycxx_test_suite_body_, id)()
/** @endcond */

/**
 * Defines a top-level test suite.
 *
 * The macro creates a namespace-scope registration function and object so suites can be declared outside
 * functions. The following block is executed immediately during static registration and adds child suites,
 * tests, and hooks to the top-level suite. Nested suites should be declared with describe().
 *
 * @param description  Human-readable suite description.
 */
#define suite(description) KAYCXX_TEST_SUITE(description, __COUNTER__)
