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
#include "test_suite.hpp"

namespace kaycxx::test::detail {

test_case::test_case(
    std::string_view description,
    callback body,
    std::source_location location,
    test_suite* parent,
    std::optional<skip_condition> skip_condition
)
    : test_node(description, parent),
      body_(std::move(body)),
      location_(location),
      skip_condition_(std::move(skip_condition))
{}

bool test_case::run(reporter& reporter) {
    reporter.before_test_case(description_);
    try {
        if (skip_condition_ && skip_condition_->should_skip()) {
            reporter.after_test_case(description_, skipped(skip_condition_->reason()));
            return true;
        }
        parent_->run_before_each_hooks();
        body_();
        parent_->run_after_each_hooks();
        reporter.after_test_case(description_);
        return true;
    } catch (detail::hook_error const& error) {
        reporter.after_test_case(description_, failure(full_description(), error, error.location()));
    } catch (kaycxx::assert::assertion_error const& error) {
        reporter.after_test_case(description_, failure(full_description(), error, location_));
    } catch (std::exception const& error) {
        reporter.after_test_case(description_, failure(full_description(), error, location_));
    } catch (...) {
        reporter.after_test_case(description_, failure(full_description(), location_));
    }
    return false;
}

void test_case::list_tests(std::vector<test_info>& tests, std::size_t& next_id) const {
    tests.push_back({std::to_string(next_id), full_description()});
    next_id++;
}

} // namespace kaycxx::test::detail
