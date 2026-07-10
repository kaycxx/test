// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "default_reporter.hpp"

#include <chrono>
#include <format>
#include <string>
#include <utility>

#include <kaycxx/term.hpp>

using namespace kaycxx::term;

namespace {

/**
 * Removes insignificant trailing zeroes from a formatted decimal number.
 *
 * @param value  Decimal number string.
 * @returns The decimal number without insignificant trailing zeroes.
 */
std::string trim_fraction(std::string value) {
    auto dot = value.find('.');
    if (dot == std::string::npos) {
        return value;
    }

    while (value.back() == '0') {
        value.pop_back();
    }

    if (value.back() == '.') {
        value.pop_back();
    }

    return value;
}

/**
 * Formats a duration for compact human-readable terminal output.
 *
 * @param duration  Duration to format.
 * @returns The formatted duration.
 */
std::string format_duration(std::chrono::nanoseconds duration) {
    auto ns = duration.count();
    auto const value = static_cast<double>(ns);

    if (ns < 1'000) {
        return std::format("{} ns", ns);
    }
    if (ns < 1'000'000) {
        return std::format("{} µs", trim_fraction(std::format("{:.2f}", value / 1'000.0)));
    }
    if (ns < 1'000'000'000) {
        return std::format("{} ms", trim_fraction(std::format("{:.2f}", value / 1'000'000.0)));
    }
    return std::format("{} s", trim_fraction(std::format("{:.2f}", value / 1'000'000'000.0)));
}

} // namespace

namespace kaycxx::test::detail {

using clock = std::chrono::steady_clock;

default_reporter::default_reporter(std::ostream& output, kaycxx::term::ansi_mode ansi_mode)
    : term_(output, ansi_mode)
{}

void default_reporter::before_test_suites(std::size_t, std::size_t num_test_cases) {
    term_ << "Starting unit tests\n\n";
    num_test_cases_ = num_test_cases;
    start_time_ = clock::now();
}

void default_reporter::before_test_suite(std::string_view description, std::size_t, std::size_t) {
    term_ << std::string(level_, ' ') << bold << description << normal_intensity << '\n';
    level_ += 2;
}

void default_reporter::before_test_case(std::string_view) {
    test_start_time_ = clock::now();
}

void default_reporter::after_test_case(std::string_view description) {
    auto end_time = clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - test_start_time_);

    term_ << std::string(level_, ' ') << fg(green) << bold << "\u2713" << " " << reset << description << " " << fg(bright_black)
        << "(" << format_duration(elapsed) << ")" << reset << '\n';
}

void default_reporter::after_test_case(std::string_view description, skipped result) {
    num_skipped_tests_++;

    term_ << std::string(level_, ' ') << fg(yellow) << bold << "-" << normal_intensity << " " << description << " (Skipped";
    if (result.reason().empty()) {
        term_ << ")";
    } else {
        term_ << ": " << result.reason() << ")";
    }
    term_ << reset << '\n';
}

void default_reporter::after_test_case(std::string_view description, failure result) {
    auto end_time = clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - test_start_time_);

    num_test_failures_++;
    failures_.push_back(std::move(result));
    term_ << std::string(level_, ' ') << fg(red) << bold << "\u2717" << normal_intensity << " " << description << " (" << failures_.size() << ")"
        << " " << fg(bright_black) << "(" << format_duration(elapsed) << ")" << reset << '\n';
}

void default_reporter::after_test_suite() {
    level_ -= 2;
    term_ << '\n';
}

void default_reporter::after_test_suite(failure result, std::size_t skipped_tests) {
    num_suite_failures_++;
    num_skipped_tests_ += skipped_tests;
    failures_.push_back(std::move(result));

    term_ << std::string(level_, ' ') << fg(red) << bold << "\u231A" << normal_intensity << " suite failed (" << failures_.size() << ")";
    if (skipped_tests > 0) {
        term_ << " " << fg(yellow) << "(" << skipped_tests << " tests skipped)" << reset;
    }
    term_ << '\n';
    level_ -= 2;
    term_ << '\n';
}

void default_reporter::after_test_suites() {
    if (!failures_.empty()) {
        term_ << "Failures:\n\n";
        int index = 1;
        for (const auto& failure : failures_) {
            auto location = failure.location();
            term_ << "  " << index << ") " << failure.description() << ":\n\n";
            term_ << "        " << fg(red) << failure.error_message() << reset << "\n\n";
            term_ << "        " << fg(bright_black) << "at " << location.file_name() << ":" << location.line() << ":" << location.column() << reset << '\n';
            index++;
            term_ << '\n';

        }
    }
    auto total = num_test_cases_;
    auto failing = num_test_failures_;
    auto skipped = num_skipped_tests_;
    auto passing = total >= failing + skipped ? total - failing - skipped : 0;
    term_ << num_test_cases_ << " tests, " << fg(green) << passing << " passing" << reset;
    if (failing > 0) {
        term_ << ", " << fg(red) << failing << " failing" << reset;
    }
    if (skipped > 0) {
        term_ << ", " << fg(yellow) << skipped << " skipped" << reset;
    }
    if (num_suite_failures_ > 0) {
        term_ << ", " << fg(red) << num_suite_failures_ << " suite failure";
        if (num_suite_failures_ != 1) {
            term_ << "s";
        }
        term_ << reset;
    }
    auto end_time = clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time_);
    term_ << "\nFinished in " << format_duration(elapsed) << '\n';
}

} // namespace kaycxx::test::detail
