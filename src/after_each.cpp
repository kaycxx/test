// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/after_each.hpp>

#include <utility>

#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void after_each(callback body, std::source_location location) {
    after_each(hook("after_each", std::move(body), location));
}

void after_each(hook value) {
    active_registry().add_after_each_hook(std::move(value));
}

} // namespace kaycxx::test
