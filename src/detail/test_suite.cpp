// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_suite.hpp"

#include <memory>
#include <utility>

#include <kaycxx/test/failure.hpp>

#include "hook_error.hpp"
#include "test_case.hpp"
#include "test_matcher.hpp"

namespace kaycxx::test::detail {

test_suite::test_suite(std::string_view description, std::source_location location)
    : test_node(description, location)
{}

test_suite::test_suite(std::string_view description, std::source_location location, test_suite& parent)
    : test_node(description, location, parent)
{}

test_suite& test_suite::add_suite(std::string_view description, std::source_location location) {
    auto suite = std::make_unique<test_suite>(description, location, *this);
    auto& suite_ref = *suite;
    children_.push_back(std::move(suite));
    return suite_ref;
}

void test_suite::add_test(std::string_view description, callback body, std::source_location location) {
    children_.push_back(std::make_unique<test_case>(description, std::move(body), location, *this));
}

void test_suite::add_test(std::string_view description, skip_condition skip_condition, callback body, std::source_location location) {
    children_.push_back(std::make_unique<test_case>(description, std::move(body), location, *this, std::move(skip_condition)));
}

void test_suite::add_before_all_hook(kaycxx::test::hook value) {
    before_all_hooks_.push_back(std::move(value));
}

void test_suite::add_before_each_hook(kaycxx::test::hook value) {
    before_each_hooks_.push_back(std::move(value));
}

void test_suite::add_after_each_hook(kaycxx::test::hook value) {
    after_each_hooks_.push_back(std::move(value));
}

void test_suite::add_after_all_hook(kaycxx::test::hook value) {
    after_all_hooks_.push_back(std::move(value));
}

void test_suite::run_before_each_hooks(run_options const& options) {
    if (parent_) {
        parent().run_before_each_hooks(options);
    }
    for (auto& hook : before_each_hooks_) {
        hook.run(options);
    }
}

void test_suite::run_after_each_hooks(run_options const& options) {
    for (auto& hook : after_each_hooks_) {
        hook.run(options);
    }
    if (parent_) {
        parent().run_after_each_hooks(options);
    }
}

bool test_suite::run(reporter& reporter, test_matcher const& matcher, run_options const& options) {
    auto const selected = counts(matcher);
    if (selected.suites == 0) {
        return true;
    }

    reporter.before_test_suite(description_, selected.suites - 1, selected.tests);

    try {
        for (auto& hook : before_all_hooks_) {
            hook.run(options);
        }
    } catch (hook_error const& error) {
        reporter.after_test_suite(failure(full_description(), error, error.location()), selected.tests);
        return false;
    }

    auto passed = true;
    for (auto& child : children_) {
        passed = child->run(reporter, matcher, options) && passed;
    }

    try {
        for (auto& hook : after_all_hooks_) {
            hook.run(options);
        }
    } catch (hook_error const& error) {
        reporter.after_test_suite(failure(full_description(), error, error.location()), 0);
        return false;
    }

    reporter.after_test_suite();
    return passed;
}

void test_suite::list_tests(std::vector<std::string>& tests, test_matcher const& matcher) const {
    for (auto const& child : children_) {
        child->list_tests(tests, matcher);
    }
}

test_counts test_suite::counts(test_matcher const& matcher) const {
    auto result = test_counts{0, 0};
    for (auto const& child : children_) {
        auto const child_counts = child->counts(matcher);
        result.suites += child_counts.suites;
        result.tests += child_counts.tests;
    }
    if (result.tests > 0 || !matcher.has_filters()) {
        result.suites++;
    }
    return result;
}

} // namespace kaycxx::test::detail
