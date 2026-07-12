// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/hook.hpp>

#include <exception>
#include <string>
#include <utility>

#include <kaycxx/assert.hpp>

#include "hook_error.hpp"

namespace kaycxx::test {

hook::hook(std::string_view name, callback body, std::source_location location)
    : body_(std::move(body)),
      name_(name),
      location_(location)
{}

void hook::run(run_options const& options) {
    if (options.break_on_exception) {
        try {
            body_();
        } catch (kaycxx::assert::assertion_error const& error) {
            throw detail::hook_error(name_ + " hook failed: " + error.what(), error.location().value_or(location_));
        }
        return;
    }

    try {
        body_();
    } catch (kaycxx::assert::assertion_error const& error) {
        throw detail::hook_error(name_ + " hook failed: " + error.what(), error.location().value_or(location_));
    } catch (std::exception const& error) {
        throw detail::hook_error(name_ + " hook failed: " + error.what(), location_);
    } catch (...) {
        throw detail::hook_error(name_ + " hook failed", location_);
    }
}

} // namespace kaycxx::test
