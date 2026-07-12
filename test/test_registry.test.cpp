// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <stdexcept>
#include <string>

#include <kaycxx/assert.hpp>
#include <kaycxx/test.hpp>

#include "support/recording_reporter.hpp"
#include "support/test_registration.hpp"

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
            registry.add_test("first", [] {});
            registry.add_suite("nested", [&] {
                registry.add_test("second", [] {});
                registry.add_test("third", [] {});
            });
        });

        auto const tests = registry.list_tests();

        assert_equal(tests.size(), 3uz);
        assert_equal(tests.at(0), "root first");
        assert_equal(tests.at(1), "root nested second");
        assert_equal(tests.at(2), "root nested third");
    });

    it("filters tests by source path and full-description pattern (special characters)", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            registry.add_test("first", [] {});
            registry.add_test("second", [] {});
        });
        auto filter = test_filter();
        filter.paths = { "missing", "test\\test_registry.test.cpp" };
        filter.name_patterns = { "second$" };

        auto const tests = registry.list_tests(filter);

        assert_equal(tests.size(), 1uz);
        assert_equal(tests.at(0), "root second");
    });

    it("combines full-description patterns with OR", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            registry.add_test("first", [] {});
            registry.add_test("second", [] {});
            registry.add_test("third", [] {});
        });
        auto filter = test_filter();
        filter.name_patterns = { "first$", "third$" };

        auto const tests = registry.list_tests(filter);

        assert_equal(tests, std::vector<std::string>{ "root first", "root third" });
    });

    it("matches test paths inherited from parent suites", [] {
        auto registry = test_registry();

        registry.add_suite("root", [&] {
            register_test_from_support(registry, "from support", [] {});
        });
        auto suite_filter = test_filter();
        suite_filter.paths = { "test_registry.test.cpp" };
        auto case_filter = test_filter();
        case_filter.paths = { "test_registration.cpp" };

        assert_equal(registry.list_tests(suite_filter).size(), 1uz);
        assert_equal(registry.list_tests(case_filter).size(), 1uz);
    });

    it("runs filtered suites once with only matching test cases", [] {
        auto registry = test_registry();
        auto root_before_all_runs = 0;
        auto root_before_each_runs = 0;
        auto nested_before_all_runs = 0;
        auto first_ran = false;
        auto second_ran = false;
        auto third_ran = false;

        registry.add_suite("root", [&] {
            registry.add_before_all_hook(hook("before_all", [&] {
                root_before_all_runs++;
            }));
            registry.add_before_each_hook(hook("before_each", [&] {
                root_before_each_runs++;
            }));
            registry.add_test("first selected", [&] {
                first_ran = true;
            });
            registry.add_test("second ignored", [&] {
                second_ran = true;
            });
            registry.add_suite("nested", [&] {
                registry.add_before_all_hook(hook("before_all", [&] {
                    nested_before_all_runs++;
                }));
                registry.add_test("third selected", [&] {
                    third_ran = true;
                });
            });
        });
        auto filter = test_filter();
        filter.name_patterns = { "selected$" };
        auto reporter = recording_reporter();

        assert_equal(registry.num_test_suites(filter), 2uz);
        assert_equal(registry.num_test_cases(filter), 2uz);
        assert_true(registry.run(reporter, filter));
        assert_equal(root_before_all_runs, 1);
        assert_equal(root_before_each_runs, 2);
        assert_equal(nested_before_all_runs, 1);
        assert_true(first_ran);
        assert_false(second_ran);
        assert_true(third_ran);
        assert_not_contain(reporter.events, std::string("before test second ignored"));
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
