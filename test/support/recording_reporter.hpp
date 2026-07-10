// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <kaycxx/test/failure.hpp>
#include <kaycxx/test/reporter.hpp>
#include <kaycxx/test/skipped.hpp>

namespace kaycxx::test::support {

class recording_reporter : public reporter {
public:
    std::vector<std::string> events;
    std::vector<failure> failures;
    std::vector<skipped> skipped_tests;
    std::size_t skipped_by_suite_failure = 0;

    void before_test_suites(std::size_t num_suites, std::size_t num_tests) override;

    void before_test_suite(std::string_view description, std::size_t num_suites, std::size_t num_tests) override;

    void before_test_case(std::string_view description) override;

    void after_test_case(std::string_view description) override;

    void after_test_case(std::string_view description, skipped result) override;

    void after_test_case(std::string_view description, failure result) override;

    void after_test_suite() override;

    void after_test_suite(failure result, std::size_t skipped_tests_count) override;

    void after_test_suites() override;
};

} // namespace kaycxx::test::support
