// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/test_registry.hpp>

#include <charconv>
#include <memory>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <kaycxx/test/suite.hpp>

#include "detail/test_suite.hpp"

namespace kaycxx::test {

test_registry::test_registry() = default;

test_registry::~test_registry() = default;

void test_registry::add_suite(std::string_view description, callback body) {
    auto& suite = add_suite_node(description);
    suite_stack_.push_back(&suite);
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

bool test_registry::run(reporter& reporter) {
    auto passed = true;
    for (auto& suite : root_suites_) {
        passed = suite->run(reporter) && passed;
    }
    return passed;
}

bool test_registry::run_test(std::string_view id, reporter& reporter) {
    std::size_t target_id = 0;
    auto const* first = id.data();
    auto const* last = first + id.size();
    auto const result = std::from_chars(first, last, target_id);
    if (result.ec != std::errc() || result.ptr != last || target_id == 0) {
        return false;
    }

    auto current_id = 0uz;
    for (auto& suite : root_suites_) {
        auto const passed = suite->run_test(target_id, current_id, reporter);
        if (current_id == target_id) {
            return passed;
        }
    }
    return false;
}

std::vector<test_info> test_registry::list_tests() const {
    auto tests = std::vector<test_info>();
    auto next_id = 1uz;
    for (auto const& suite : root_suites_) {
        suite->list_tests(tests, next_id);
    }
    return tests;
}

std::size_t test_registry::num_test_suites() const {
    std::size_t count = 0;
    for (auto const& suite : root_suites_) {
        count = count + 1 + suite->num_test_suites();
    }
    return count;
}

std::size_t test_registry::num_test_cases() const {
    std::size_t count = 0;
    for (auto const& suite : root_suites_) {
        count += suite->num_test_cases();
    }
    return count;
}

detail::test_suite& test_registry::current() {
    if (suite_stack_.empty()) {
        throw std::logic_error("No active test suite");
    }
    return *suite_stack_.back();
}

detail::test_suite& test_registry::add_suite_node(std::string_view description) {
    if (suite_stack_.empty()) {
        auto suite = std::make_unique<detail::test_suite>(description);
        auto& suite_ref = *suite;
        root_suites_.push_back(std::move(suite));
        return suite_ref;
    }
    return current().add_suite(description);
}

test_registry& default_registry() {
    static auto registry = test_registry();
    return registry;
}

namespace {

/**
 * Stores the registry currently receiving DSL registrations.
 *
 * @returns Reference to the active registry pointer.
 */
test_registry*& active_registry_pointer() {
    static auto* registry = &default_registry();
    return registry;
}

} // namespace

test_registry& active_registry() {
    return *active_registry_pointer();
}

test_registry& set_active_registry(test_registry& registry) {
    auto& previous = active_registry();
    active_registry_pointer() = &registry;
    return previous;
}

void reset_active_registry() {
    active_registry_pointer() = &default_registry();
}

namespace detail {

void define_root_suite(std::string_view description, callback body) {
    active_registry().add_suite(description, std::move(body));
}

} // namespace detail

} // namespace kaycxx::test
