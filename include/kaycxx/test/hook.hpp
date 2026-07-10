// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Defines registered test hooks.
 */

#include <source_location>
#include <string>
#include <string_view>

#include <kaycxx/test/callback.hpp>

namespace kaycxx::test {

/**
 * Registered setup or teardown hook.
 *
 * The hook stores the registration source location so hook failures can be reported at the user code that registered the hook.
 */
class hook {
public:
    /**
     * Creates a hook.
     *
     * @param name      Hook name used in failure messages.
     * @param body      Hook callback to execute.
     * @param location  Source location used when the hook throws an exception without its own location.
     */
    explicit hook(std::string_view name, callback body, std::source_location location = std::source_location::current());

    /** Runs the hook and wraps thrown exceptions in a hook error with source location information. */
    void run();

private:
    /** Hook callback to execute. */
    callback body_;

    /** Hook name used in failure messages. */
    std::string name_;

    /** Source location of the hook registration call. */
    std::source_location location_;
};

} // namespace kaycxx::test
