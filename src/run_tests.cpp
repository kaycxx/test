// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/run_tests.hpp>

#include <kaycxx/cli.hpp>

#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <string_view>

#include "detail/default_reporter.hpp"

namespace kaycxx::test {

namespace {

/** Command-line value containing a compiled regular expression. */
class regex_pattern {
public:
    /**
     * Compiles a regular expression from its command-line representation.
     *
     * @param pattern  Regular expression to compile.
     *
     * @throws std::regex_error  When the pattern is not a valid regular expression.
     */
    explicit regex_pattern(std::string_view pattern)
        : value_(std::string(pattern)) {
    }

    /**
     * Returns the compiled regular expression.
     *
     * @returns Compiled regular expression.
     */
    std::regex const& value() const noexcept {
        return value_;
    }

private:
    /** Compiled regular expression. */
    std::regex value_;
};

/**
 * Quotes a value as a CMake bracket argument.
 *
 * @param value  Value to quote.
 * @returns CMake bracket argument.
 */
std::string cmake_quote(std::string_view value) {
    auto equals = std::string();
    while (value.contains("]" + equals + "]")) {
        equals += "=";
    }
    return "[" + equals + "[" + std::string(value) + "]" + equals + "]";
}

/**
 * Escapes a literal value for use in an ECMAScript regular expression.
 *
 * @param value  Literal value to escape.
 * @returns Escaped regular expression.
 */
std::string regex_escape(std::string_view value) {
    auto result = std::string();
    result.reserve(value.size());
    for (auto const character : value) {
        if (std::string_view("\\.^$|()[]{}*+?").contains(character)) {
            result += '\\';
        }
        result += character;
    }
    return result;
}

/**
 * Writes a CTest include file for the registered tests.
 *
 * @param registry    Registry containing the tests to write.
 * @param file_name   CTest include file to write.
 * @param executable  Test executable used by the generated test commands.
 * @returns True when the file was written successfully.
 */
bool write_ctest_file(test_registry const& registry, std::string_view file_name, std::string_view executable) {
    auto output = std::ofstream(std::string(file_name));
    if (!output) {
        return false;
    }

    for (auto const& description : registry.list_tests()) {
        auto const pattern = "^" + regex_escape(description) + "$";
        output << "add_test(" << cmake_quote(description) << " " << cmake_quote(executable) << " --test-name-pattern " << cmake_quote(pattern) << ")\n";
    }
    return true;
}

/**
 * Runs tests selected by a filter with the default reporter.
 *
 * @param registry   Registry containing the tests to run.
 * @param output     Output stream receiving test output.
 * @param ansi_mode  ANSI output mode.
 * @param filter     Test selection filter.
 * @returns Process exit code.
 */
int run_filtered_tests(test_registry& registry, std::ostream& output, kaycxx::term::ansi_mode ansi_mode, test_filter const& filter) {
    auto const num_tests = registry.num_test_cases(filter);
    if (num_tests == 0 && (!filter.paths.empty() || !filter.name_patterns.empty())) {
        output << "No tests matched\n";
        return 1;
    }

    auto reporter = detail::default_reporter(output, ansi_mode);
    reporter.before_test_suites(registry.num_test_suites(filter), num_tests);
    auto const passed = registry.run(reporter, filter);
    reporter.after_test_suites();
    return passed ? 0 : 1;
}

} // namespace

int run_tests() {
    return run_tests(std::cout);
}

int run_tests(int argc, char* argv[]) {
    auto const command_name = argc > 0 ? std::string_view(argv[0]) : std::string_view("test");
    auto app = kaycxx::cli::command(command_name, {
        .description = "Runs registered unit tests."
    });
    auto help = app.flag("help", "Show this help").action();
    auto ctest_file = app.option<std::string>("write-ctest", "FILE", "Write registered tests to a CTest include file").action();
    auto name_patterns = app.repeatable_option<regex_pattern>("test-name-pattern", 't', "PATTERN", "Run tests whose full description matches PATTERN");
    auto paths = app.parameters<std::string>("PATH", "Run tests from matching files or directories");
    auto filter = test_filter();

    try {
        auto arguments = app.parse(argc, argv);
        if (arguments.get(help)) {
            return app.print_help();
        }

        arguments.validate();
        filter.paths = arguments.get(paths);
        if (arguments.has(name_patterns)) {
            auto const& patterns = arguments.get(name_patterns);
            filter.name_patterns.reserve(patterns.size());
            for (auto const& pattern : patterns) {
                filter.name_patterns.push_back(pattern.value());
            }
        }

        if (arguments.has(ctest_file)) {
            auto const& file_name = arguments.get(ctest_file);
            if (!write_ctest_file(default_registry(), file_name, command_name)) {
                std::cerr << "Unable to write CTest file: " << file_name << '\n';
                return 1;
            }
            return 0;
        }
    } catch (kaycxx::cli::parse_error const& error) {
        std::cerr << app.name() << ": " << error.what() << '\n';
        std::cerr << "Try '" << app.name() << " --help' for more information.\n";
        return 2;
    }

    return run_filtered_tests(default_registry(), std::cout, kaycxx::term::ansi_mode::automatic, filter);
}

int run_tests(std::ostream& output) {
    return run_tests(output, kaycxx::term::ansi_mode::automatic);
}

int run_tests(std::ostream& output, kaycxx::term::ansi_mode ansi_mode) {
    return run_tests(default_registry(), output, ansi_mode);
}

int run_tests(test_registry& registry, std::ostream& output) {
    return run_tests(registry, output, kaycxx::term::ansi_mode::automatic);
}

int run_tests(test_registry& registry, std::ostream& output, kaycxx::term::ansi_mode ansi_mode) {
    auto reporter = detail::default_reporter(output, ansi_mode);
    reporter.before_test_suites(registry.num_test_suites(), registry.num_test_cases());
    auto const passed = registry.run(reporter);
    reporter.after_test_suites();
    return passed ? 0 : 1;
}

} // namespace kaycxx::test
