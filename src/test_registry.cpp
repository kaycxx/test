// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/test_registry.hpp>

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include <kaycxx/test/suite.hpp>

#include "detail/test_matcher.hpp"
#include "detail/test_suite.hpp"

namespace kaycxx::test {

test_registry::test_registry() = default;

test_registry::~test_registry() = default;

void test_registry::add_suite(std::string_view description, callback body, std::source_location location) {
    auto& suite = add_suite_node(description, location);
    suite_stack_.push_back(suite);
    try {
        body();
        suite_stack_.pop_back();
    } catch (...) {
        suite_stack_.pop_back();
        throw;
    }
}

void test_registry::add_test(std::string_view description, callback body, std::source_location location) {
    current().add_test(description, std::move(body), location);
}

void test_registry::add_test(std::string_view description, skip_condition condition, callback body, std::source_location location) {
    current().add_test(description, std::move(condition), std::move(body), location);
}

void test_registry::add_before_all_hook(hook value) {
    current().add_before_all_hook(std::move(value));
}

void test_registry::add_before_each_hook(hook value) {
    current().add_before_each_hook(std::move(value));
}

void test_registry::add_after_each_hook(hook value) {
    current().add_after_each_hook(std::move(value));
}

void test_registry::add_after_all_hook(hook value) {
    current().add_after_all_hook(std::move(value));
}

bool test_registry::run(reporter& reporter, run_options const& options) {
    return run(reporter, test_filter(), options);
}

bool test_registry::run(reporter& reporter, test_filter const& filter, run_options const& options) {
    auto const matcher = detail::test_matcher(filter);
    auto passed = true;
    for (auto& suite : root_suites_) {
        passed = suite->run(reporter, matcher, options) && passed;
    }
    return passed;
}

std::vector<std::string> test_registry::list_tests() const {
    return list_tests(test_filter());
}

std::vector<std::string> test_registry::list_tests(test_filter const& filter) const {
    auto const matcher = detail::test_matcher(filter);
    auto tests = std::vector<std::string>();
    for (auto const& suite : root_suites_) {
        suite->list_tests(tests, matcher);
    }

    auto descriptions = std::unordered_set<std::string>();
    auto result = std::vector<std::string>();
    result.reserve(tests.size());
    for (auto& test : tests) {
        if (descriptions.insert(test).second) {
            result.push_back(std::move(test));
        }
    }
    return result;
}

std::size_t test_registry::num_test_suites() const {
    return num_test_suites(test_filter());
}

std::size_t test_registry::num_test_suites(test_filter const& filter) const {
    auto const matcher = detail::test_matcher(filter);
    std::size_t count = 0;
    for (auto const& suite : root_suites_) {
        count += suite->counts(matcher).suites;
    }
    return count;
}

std::size_t test_registry::num_test_cases() const {
    return num_test_cases(test_filter());
}

std::size_t test_registry::num_test_cases(test_filter const& filter) const {
    auto const matcher = detail::test_matcher(filter);
    std::size_t count = 0;
    for (auto const& suite : root_suites_) {
        count += suite->counts(matcher).tests;
    }
    return count;
}

detail::test_suite& test_registry::current() {
    if (suite_stack_.empty()) {
        throw std::logic_error("No active test suite");
    }
    return suite_stack_.back().get();
}

detail::test_suite& test_registry::add_suite_node(std::string_view description, std::source_location location) {
    if (suite_stack_.empty()) {
        auto suite = std::make_unique<detail::test_suite>(description, location);
        auto& suite_ref = *suite;
        root_suites_.push_back(std::move(suite));
        return suite_ref;
    }
    return current().add_suite(description, location);
}

test_registry& default_registry() {
    static auto registry = test_registry();
    return registry;
}

namespace {

/**
 * Stores the registry currently receiving DSL registrations.
 *
 * @returns Reference to the active registry reference.
 */
std::reference_wrapper<test_registry>& active_registry_reference() {
    static auto registry = std::ref(default_registry());
    return registry;
}

} // namespace

test_registry& active_registry() {
    return active_registry_reference().get();
}

test_registry& set_active_registry(test_registry& registry) {
    auto& previous = active_registry();
    active_registry_reference() = registry;
    return previous;
}

void reset_active_registry() {
    active_registry_reference() = default_registry();
}

namespace detail {

void define_root_suite(std::string_view description, callback body, std::source_location location) {
    active_registry().add_suite(description, std::move(body), location);
}

} // namespace detail

} // namespace kaycxx::test
