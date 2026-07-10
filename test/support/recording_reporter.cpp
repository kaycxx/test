// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "recording_reporter.hpp"

#include <string>
#include <utility>

namespace kaycxx::test::support {

void recording_reporter::before_test_suites(std::size_t, std::size_t) {
    events.push_back("before all");
}

void recording_reporter::before_test_suite(std::string_view description, std::size_t, std::size_t) {
    events.push_back("before suite " + std::string(description));
}

void recording_reporter::before_test_case(std::string_view description) {
    events.push_back("before test " + std::string(description));
}

void recording_reporter::after_test_case(std::string_view description) {
    events.push_back("pass test " + std::string(description));
}

void recording_reporter::after_test_case(std::string_view description, skipped result) {
    events.push_back("skip test " + std::string(description));
    skipped_tests.push_back(std::move(result));
}

void recording_reporter::after_test_case(std::string_view description, failure result) {
    events.push_back("fail test " + std::string(description));
    failures.push_back(std::move(result));
}

void recording_reporter::after_test_suite() {
    events.push_back("pass suite");
}

void recording_reporter::after_test_suite(failure result, std::size_t skipped_tests_count) {
    events.push_back("fail suite");
    failures.push_back(std::move(result));
    skipped_by_suite_failure += skipped_tests_count;
}

void recording_reporter::after_test_suites() {
    events.push_back("after all");
}

} // namespace kaycxx::test::support
