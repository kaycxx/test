// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include "test_case.hpp"

#include <exception>
#include <string>
#include <utility>

#include <kaycxx/assert.hpp>
#include <kaycxx/test/failure.hpp>
#include <kaycxx/test/skipped.hpp>

#include "hook_error.hpp"
#include "test_matcher.hpp"
#include "test_suite.hpp"

namespace kaycxx::test::detail {

test_case::test_case(
    std::string_view description,
    callback body,
    std::source_location location,
    test_suite& parent,
    std::optional<skip_condition> skip_condition
)
    : test_node(description, location, parent),
      body_(std::move(body)),
      skip_condition_(std::move(skip_condition))
{}

bool test_case::run(reporter& reporter, test_matcher const& matcher, run_options const& options) {
    return !matches(matcher) || execute(reporter, options);
}

bool test_case::execute(reporter& reporter, run_options const& options) {
    reporter.before_test_case(description_);

    auto execute_with_assertion_handling = [&] {
        try {
            if (skip_condition_ && skip_condition_->should_skip()) {
                reporter.after_test_case(description_, skipped(skip_condition_->reason()));
                return true;
            }
            parent().run_before_each_hooks(options);
            body_();
            parent().run_after_each_hooks(options);
            reporter.after_test_case(description_);
            return true;
        } catch (detail::hook_error const& error) {
            reporter.after_test_case(description_, failure(full_description(), error, error.location()));
        } catch (kaycxx::assert::assertion_error const& error) {
            reporter.after_test_case(description_, failure(full_description(), error, location_));
        }
        return false;
    };

    if (options.break_on_exception) {
        return execute_with_assertion_handling();
    }

    try {
        return execute_with_assertion_handling();
    } catch (std::exception const& error) {
        reporter.after_test_case(description_, failure(full_description(), error, location_));
        return false;
    } catch (...) {
        reporter.after_test_case(description_, failure(full_description(), location_));
        return false;
    }
}

bool test_case::matches(test_matcher const& matcher) const {
    return matches_path(matcher) && matcher.matches_name(full_description());
}

void test_case::list_tests(std::vector<std::string>& tests, test_matcher const& matcher) const {
    if (matches(matcher)) {
        tests.push_back(full_description());
    }
}

test_counts test_case::counts(test_matcher const& matcher) const {
    return {0, matches(matcher) ? 1uz : 0uz};
}

} // namespace kaycxx::test::detail
