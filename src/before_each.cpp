// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/before_each.hpp>

#include <utility>

#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void before_each(callback body, std::source_location location) {
    before_each(hook("before_each", std::move(body), location));
}

void before_each(hook value) {
    active_registry().add_before_each_hook(std::move(value));
}

} // namespace kaycxx::test
