// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include <string>
#include <vector>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/recording_reporter.hpp"

using namespace kaycxx::assert;
using namespace kaycxx::test;
using namespace kaycxx::test::support;

suite("test_registry") {
    it("uses the default registry as active registry by default", [] {
        reset_active_registry();

        assert_equal(&active_registry(), &default_registry());
    });

    it("allows changing and resetting the active registry", [] {
        auto registry = test_registry();

        auto& previous = set_active_registry(registry);

        assert_equal(&previous, &default_registry());
        assert_equal(&active_registry(), &registry);

        reset_active_registry();

        assert_equal(&active_registry(), &default_registry());
    });

    it("counts top-level and nested suites and test cases", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            registry.add_test("first", [] {});
            registry.add_suite("nested", [&] {
                registry.add_test("second", [] {});
                registry.add_test("third", [] {});
            });
        });

        assert_equal(registry.num_test_suites(), 2uz);
        assert_equal(registry.num_test_cases(), 3uz);
    });

    it("lists registered tests in execution order", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            registry.add_test("first", [] {});
            registry.add_suite("nested", [&] {
                registry.add_test("second", [] {});
                registry.add_test("third", [] {});
            });
        });

        auto const tests = registry.list_tests();

        assert_equal(tests.size(), 3uz);
        assert_equal(tests.at(0).id, "1");
        assert_equal(tests.at(0).description, "root first");
        assert_equal(tests.at(1).id, "2");
        assert_equal(tests.at(1).description, "root nested second");
        assert_equal(tests.at(2).id, "3");
        assert_equal(tests.at(2).description, "root nested third");
    });

    it("runs one selected test by id", [] {
        auto registry = test_registry();
        auto execution = std::vector<std::string>();

        registry.add_suite("root", [&] {
            registry.add_before_all_hook(hook("before_all", [&] {
                execution.push_back("root before_all");
            }));
            registry.add_before_each_hook(hook("before_each", [&] {
                execution.push_back("root before_each");
            }));
            registry.add_test("first", [&] {
                execution.push_back("first");
            });
            registry.add_suite("nested", [&] {
                registry.add_before_each_hook(hook("before_each", [&] {
                    execution.push_back("nested before_each");
                }));
                registry.add_test("second", [&] {
                    execution.push_back("second");
                });
            });
        });
        auto reporter = recording_reporter();

        assert_true(registry.run_test("2", reporter));

        assert_equal(execution.size(), 4uz);
        assert_equal(execution.at(0), "root before_all");
        assert_equal(execution.at(1), "root before_each");
        assert_equal(execution.at(2), "nested before_each");
        assert_equal(execution.at(3), "second");
        assert_equal(reporter.events.at(0), "before suite root");
        assert_equal(reporter.events.at(1), "before suite nested");
        assert_equal(reporter.events.at(2), "before test second");
        assert_equal(reporter.events.at(3), "pass test second");
    });

    it("rejects unknown selected test ids", [] {
        auto registry = test_registry();
        auto reporter = recording_reporter();

        registry.add_suite("root", [&] {
            registry.add_test("first", [] {});
        });

        assert_false(registry.run_test("unknown", reporter));
        assert_true(reporter.events.empty());
    });

    it("rejects test cases outside of suites", [] {
        auto registry = test_registry();

        assert_throw<std::logic_error>([&] {
            registry.add_test("orphan", [] {});
        }, "No active test suite");
    });

    it("rejects hooks outside of suites", [] {
        auto registry = test_registry();

        assert_throw<std::logic_error>([&] {
            registry.add_before_each_hook(hook("before_each", [] {}));
        }, "No active test suite");
    });

    it("restores the active suite after failed nested suite registration", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            assert_throw<std::runtime_error>([&] {
                registry.add_suite("broken", [] {
                    throw std::runtime_error("registration failed");
                });
            }, "registration failed");

            registry.add_test("after failure", [] {});
        });

        assert_equal(registry.num_test_cases(), 1uz);
    });
}
