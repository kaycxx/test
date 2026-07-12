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

test_suite::test_suite(std::string_view description, std::source_location location, test_suite* parent)
    : test_node(description, location, parent)
{}

test_suite& test_suite::add_suite(std::string_view description, std::source_location location) {
    auto suite = std::make_unique<test_suite>(description, location, this);
    auto& suite_ref = *suite;
    children_.push_back(std::move(suite));
    return suite_ref;
}

void test_suite::add_test(std::string_view description, callback body, std::source_location location) {
    children_.push_back(std::make_unique<test_case>(description, std::move(body), location, this));
}

void test_suite::add_test(std::string_view description, skip_condition skip_condition, callback body, std::source_location location) {
    children_.push_back(std::make_unique<test_case>(description, std::move(body), location, this, std::move(skip_condition)));
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

void test_suite::run_before_each_hooks() {
    if (parent_) {
        parent_->run_before_each_hooks();
    }
    for (auto& hook : before_each_hooks_) {
        hook.run();
    }
}

void test_suite::run_after_each_hooks() {
    for (auto& hook : after_each_hooks_) {
        hook.run();
    }
    if (parent_) {
        parent_->run_after_each_hooks();
    }
}

bool test_suite::run(reporter& reporter) {
    return run(reporter, nullptr);
}

bool test_suite::run(reporter& reporter, test_matcher const& matcher) {
    return run(reporter, &matcher);
}

bool test_suite::run(reporter& reporter, test_matcher const* matcher) {
    auto const num_suites = matcher == nullptr ? num_test_suites() : num_test_suites(*matcher);
    auto const num_tests = matcher == nullptr ? num_test_cases() : num_test_cases(*matcher);
    if (matcher != nullptr && num_tests == 0) {
        return true;
    }

    reporter.before_test_suite(description_, num_suites, num_tests);

    try {
        for (auto& hook : before_all_hooks_) {
            hook.run();
        }
    } catch (hook_error const& error) {
        reporter.after_test_suite(failure(full_description(), error, error.location()), num_tests);
        return false;
    }

    auto passed = true;
    for (auto& child : children_) {
        auto* suite = dynamic_cast<test_suite*>(child.get());
        if (suite != nullptr) {
            passed = (matcher == nullptr ? suite->run(reporter) : suite->run(reporter, *matcher)) && passed;
            continue;
        }

        auto& test = dynamic_cast<test_case&>(*child);
        if (matcher == nullptr || test.matches(*matcher)) {
            passed = test.run(reporter) && passed;
        }
    }

    try {
        for (auto& hook : after_all_hooks_) {
            hook.run();
        }
    } catch (hook_error const& error) {
        reporter.after_test_suite(failure(full_description(), error, error.location()), 0);
        return false;
    }

    reporter.after_test_suite();
    return passed;
}

bool test_suite::run_test(std::size_t target_id, std::size_t& current_id, reporter& reporter) {
    auto probe_id = current_id;
    auto const selected_suites = selected_suite_count(target_id, probe_id);
    if (!selected_suites) {
        current_id = probe_id;
        return true;
    }

    reporter.before_test_suite(description_, *selected_suites, 1);

    try {
        for (auto& hook : before_all_hooks_) {
            hook.run();
        }
    } catch (hook_error const& error) {
        current_id = probe_id;
        reporter.after_test_suite(failure(full_description(), error, error.location()), 1);
        return false;
    }

    auto passed = true;
    for (auto& child : children_) {
        auto* suite = dynamic_cast<test_suite*>(child.get());
        if (suite != nullptr) {
            passed = suite->run_test(target_id, current_id, reporter);
            if (current_id == target_id) {
                break;
            }
        } else {
            current_id++;
            if (current_id == target_id) {
                passed = child->run(reporter);
                break;
            }
        }
    }

    try {
        for (auto& hook : after_all_hooks_) {
            hook.run();
        }
    } catch (hook_error const& error) {
        reporter.after_test_suite(failure(full_description(), error, error.location()), 0);
        return false;
    }

    reporter.after_test_suite();
    return passed;
}

void test_suite::list_tests(std::vector<test_info>& tests, std::size_t& next_id) const {
    for (auto const& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            suite->list_tests(tests, next_id);
        } else {
            dynamic_cast<test_case const&>(*child).list_tests(tests, next_id);
        }
    }
}

void test_suite::list_tests(std::vector<test_info>& tests, std::size_t& next_id, test_matcher const& matcher) const {
    for (auto const& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            suite->list_tests(tests, next_id, matcher);
            continue;
        }

        auto const& test = dynamic_cast<test_case const&>(*child);
        if (test.matches(matcher)) {
            test.list_tests(tests, next_id);
        } else {
            next_id++;
        }
    }
}

std::size_t test_suite::num_test_suites() const {
    std::size_t count = 0;
    for (auto& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            count = count + 1 + suite->num_test_suites();
        }
    }
    return count;
}

std::size_t test_suite::num_test_suites(test_matcher const& matcher) const {
    std::size_t count = 0;
    for (auto const& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr && suite->num_test_cases(matcher) > 0) {
            count = count + 1 + suite->num_test_suites(matcher);
        }
    }
    return count;
}

std::size_t test_suite::num_test_cases() const {
    std::size_t count = 0;
    for (auto& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            count = count + suite->num_test_cases();
        } else {
            count++;
        }
    }
    return count;
}

std::size_t test_suite::num_test_cases(test_matcher const& matcher) const {
    std::size_t count = 0;
    for (auto const& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            count += suite->num_test_cases(matcher);
        } else if (dynamic_cast<test_case const&>(*child).matches(matcher)) {
            count++;
        }
    }
    return count;
}

std::optional<std::size_t> test_suite::selected_suite_count(std::size_t target_id, std::size_t& current_id) const {
    for (auto const& child : children_) {
        auto const* suite = dynamic_cast<test_suite const*>(child.get());
        if (suite != nullptr) {
            auto const nested_suites = suite->selected_suite_count(target_id, current_id);
            if (nested_suites) {
                return *nested_suites + 1;
            }
        } else {
            current_id++;
            if (current_id == target_id) {
                return 0;
            }
        }
    }
    return std::nullopt;
}

} // namespace kaycxx::test::detail
