// SPDX-FileCopyrightText: 2026 Klaus Reimer <k@ailis.de>
// SPDX-License-Identifier: MIT

#include <kaycxx/test/failure.hpp>

namespace kaycxx::test {

failure::failure(
    std::string_view description,
    kaycxx::assert::assertion_error const& error,
    std::source_location location
) : description_(description),
    error_message_(error.what()),
    location_(error.location().value_or(location)),
    actual_(error.actual()),
    expected_(error.expected())
{}

failure::failure(
    std::string_view description,
    std::exception const& error,
    std::source_location location
) : description_(description),
    error_message_(error.what()),
    location_(location)
{}

failure::failure(
    std::string_view description,
    std::source_location location
) : description_(description),
    error_message_("Unknown exception"),
    location_(location)
{}

std::string const& failure::description() const noexcept {
    return description_;
}

std::string const& failure::error_message() const noexcept {
    return error_message_;
}

std::source_location const& failure::location() const noexcept {
    return location_;
}

std::optional<std::string> const& failure::actual() const noexcept {
    return actual_;
}

std::optional<std::string> const& failure::expected() const noexcept {
    return expected_;
}

} // namespace kaycxx::test
