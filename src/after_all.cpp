// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/after_all.hpp>

#include <utility>

#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/test_registry.hpp>

namespace kaycxx::test {

void after_all(callback body, std::source_location location) {
    after_all(hook("after_all", std::move(body), location));
}

void after_all(hook value) {
    active_registry().add_after_all_hook(std::move(value));
}

} // namespace kaycxx::test
