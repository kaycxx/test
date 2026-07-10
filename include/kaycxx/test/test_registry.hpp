// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines isolated test registries.
 */

#include <cstddef>
#include <memory>
#include <source_location>
#include <string>
#include <string_view>
#include <vector>

#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/reporter.hpp>
#include <kaycxx/test/skip.hpp>

namespace kaycxx::test::detail {

class test_suite;

} // namespace kaycxx::test::detail

namespace kaycxx::test {

/**
 * Describes a registered test case.
 */
struct test_info {
    /** Stable numeric test id within the current binary. */
    std::string id;

    /** Full human-readable test description. */
    std::string description;
};

/**
 * Owns a set of test suites and the registration stack used while suite bodies execute.
 */
class test_registry {
public:
    /** Creates an empty test registry. */
    test_registry();

    /** Destroys the test registry and all registered suites. */
    ~test_registry();

    test_registry(test_registry const&) = delete;
    test_registry& operator=(test_registry const&) = delete;

    /**
     * Adds a suite and immediately executes its registration body.
     *
     * If no suite is currently active, this adds a top-level suite. Otherwise it adds a nested suite to the current suite.
     *
     * @param description  Human-readable suite description.
     * @param body         Registration callback that defines child suites, tests, and hooks.
     */
    void add_suite(std::string_view description, callback body);

    /**
     * Adds a test case to the current suite.
     *
     * @param description  Human-readable test description.
     * @param body         Test callback to execute.
     * @param location     Source location of the test registration.
     */
    void add_test(std::string_view description, callback body, std::source_location location = std::source_location::current());

    /**
     * Adds a test case with a skip condition to the current suite.
     *
     * @param description  Human-readable test description.
     * @param condition    Condition deciding if the test should be skipped.
     * @param body         Test callback to execute.
     * @param location     Source location of the test registration.
     */
    void add_test(std::string_view description, skip_condition condition, callback body, std::source_location location = std::source_location::current());

    /**
     * Adds a before_all hook to the current suite.
     *
     * @param value  Hook to register.
     */
    void add_before_all_hook(hook value);

    /**
     * Adds a before_each hook to the current suite.
     *
     * @param value  Hook to register.
     */
    void add_before_each_hook(hook value);

    /**
     * Adds an after_each hook to the current suite.
     *
     * @param value  Hook to register.
     */
    void add_after_each_hook(hook value);

    /**
     * Adds an after_all hook to the current suite.
     *
     * @param value  Hook to register.
     */
    void add_after_all_hook(hook value);

    /**
     * Executes all registered top-level suites in declaration order.
     *
     * @param reporter  Reporter receiving lifecycle events.
     * @returns True if all suites passed, false if any suite failed.
     */
    bool run(reporter& reporter);

    /**
     * Executes one registered test case by id.
     *
     * @param id        Test id from list_tests().
     * @param reporter  Reporter receiving lifecycle events.
     * @returns True if the selected test passed, false if it failed or the id was unknown.
     */
    bool run_test(std::string_view id, reporter& reporter);

    /**
     * Lists all registered test cases in execution order.
     *
     * @returns Registered test cases.
     */
    std::vector<test_info> list_tests() const;

    /**
     * Returns the number of all registered top-level and nested suites.
     *
     * @returns The number of all registered top-level and nested suites.
     */
    std::size_t num_test_suites() const;

    /**
     * Returns the number of all registered test cases.
     *
     * @returns The number of all registered test cases.
     */
    std::size_t num_test_cases() const;

private:
    /** Top-level suites in registration order. */
    std::vector<std::unique_ptr<detail::test_suite>> root_suites_;

    /** Active suite stack used while registration callbacks execute. */
    std::vector<detail::test_suite*> suite_stack_;

    /**
     * Returns the suite currently receiving registrations.
     *
     * @returns The active suite.
     */
    detail::test_suite& current();

    /**
     * Adds a top-level or nested suite without running a registration body.
     *
     * @param description  Human-readable suite description.
     * @returns The created suite.
     */
    detail::test_suite& add_suite_node(std::string_view description);
};

/**
 * Returns the global registry used by the public DSL functions.
 *
 * @returns The default test registry.
 */
test_registry& default_registry();

/**
 * Returns the registry currently used by the public DSL functions.
 *
 * The active registry is the default registry unless it has been changed explicitly. This is mainly useful for
 * self-tests and custom embedding scenarios that need to collect DSL registrations in an isolated registry.
 *
 * @returns The active test registry.
 */
test_registry& active_registry();

/**
 * Changes the registry used by the public DSL functions.
 *
 * @param registry  Registry to use for following DSL registrations.
 * @returns The previously active registry.
 */
test_registry& set_active_registry(test_registry& registry);

/** Resets the active registry to the default registry. */
void reset_active_registry();

} // namespace kaycxx::test
