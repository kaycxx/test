// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#pragma once

/**
 * @file
 * Includes the complete public test API.
 */

/** Root namespace for kaycxx libraries. */
namespace kaycxx {

/** Unit test framework functions and types. */
namespace test {}

} // namespace kaycxx

#include <kaycxx/test/after_all.hpp>
#include <kaycxx/test/after_each.hpp>
#include <kaycxx/test/before_all.hpp>
#include <kaycxx/test/before_each.hpp>
#include <kaycxx/test/callback.hpp>
#include <kaycxx/test/describe.hpp>
#include <kaycxx/test/failure.hpp>
#include <kaycxx/test/hook.hpp>
#include <kaycxx/test/it.hpp>
#include <kaycxx/test/run_options.hpp>
#include <kaycxx/test/run_tests.hpp>
#include <kaycxx/test/skip.hpp>
#include <kaycxx/test/skipped.hpp>
#include <kaycxx/test/suite.hpp>
#include <kaycxx/test/test_filter.hpp>
#include <kaycxx/test/test_registry.hpp>
