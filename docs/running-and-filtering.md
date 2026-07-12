# Running and Filtering

The `kaycxx::test` static library supplies `main()` for a test executable. Running that executable executes all registered tests and prints the default report.

```sh
./my-project-tests
```

The report preserves the suite hierarchy, shows individual test durations, lists failures with source locations, summarizes passed, failed, and skipped tests, and prints the complete run time.

ANSI colors are selected automatically. A non-empty `FORCE_COLOR` environment variable forces formatting for redirected output and CI logs. A non-empty `NO_COLOR` environment variable disables formatting. `FORCE_COLOR` takes priority when both environment variables are set.

## Path Filters

Positional command-line arguments select tests by source file or directory. Matching is a platform-independent substring check against the source locations of each test and its parent suites. A filter does not need to be a complete path. A file name such as `parser.test.cpp` or even a distinctive fragment such as `parser.test` also matches a test registered from `test/unit/parser.test.cpp`.

```sh
./my-project-tests test/parser.test.cpp
./my-project-tests parser.test.cpp
./my-project-tests parser.test
./my-project-tests test/integration
```

Multiple path filters are combined with OR.

```sh
./my-project-tests test/parser test/formatter
```

## Description Filters

`-t` and `--test-name-pattern` select tests by their full description using ECMAScript regular expressions. The option is repeatable and patterns are combined with OR.

```sh
./my-project-tests -t 'handles empty input'
./my-project-tests -t '^parser '
./my-project-tests -t 'rejects invalid input$' -t 'handles empty input$'
```

The match uses regular-expression search. A pattern without special regular-expression characters works as a simple substring search. Add `^` and `$` when the complete full description must match.

Path filters and description patterns are combined with AND.

```sh
./my-project-tests test/unit -t '^parser '
```

Only suites containing selected tests are executed and reported. Their hooks run normally for the selected tests. A non-empty filter that matches no tests prints `No tests matched` and exits with status 1.

## Command-Line Help

Use `--help` to display the test executable's command usage, available options, and positional path filters.

```sh
./my-project-tests --help
```

## Exit Codes

- `0` means all selected tests passed, skipped, or a requested action completed successfully.
- `1` means a test or suite failed, no tests matched a non-empty filter, or the CTest file could not be written.
- `2` means command-line parsing failed or a test-name regular expression was invalid.
