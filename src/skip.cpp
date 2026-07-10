// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/skip.hpp>

#include <utility>

namespace kaycxx::test {

skip_condition::skip_condition(std::string_view reason)
    : reason_(reason)
{}

skip_condition::skip_condition(std::move_only_function<bool()> condition, std::string_view reason)
    : condition_(std::move(condition)),
      reason_(reason)
{}

bool skip_condition::should_skip() {
    if (!condition_) {
        return true;
    }
    return condition_();
}

std::string const& skip_condition::reason() const noexcept {
    return reason_;
}

skip_condition skip(std::string_view reason) {
    return skip_condition(reason);
}

skip_condition skip_if(std::move_only_function<bool()> condition, std::string_view reason) {
    return skip_condition(std::move(condition), reason);
}

skip_condition skip_unless(std::move_only_function<bool()> condition, std::string_view reason) {
    if (!condition) {
        return skip_condition(reason);
    }
    return skip_condition([condition = std::move(condition)]() mutable {
        return !condition();
    }, reason);
}

} // namespace kaycxx::test
