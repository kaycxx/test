// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/before_all.hpp>

#include <utility>

#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void before_all(callback body, std::source_location location) {
    before_all(hook("before_all", std::move(body), location));
}

void before_all(hook value) {
    active_registry().add_before_all_hook(std::move(value));
}

} // namespace kaycxx::test
