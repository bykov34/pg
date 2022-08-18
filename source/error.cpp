#include <pg/error.hpp>

namespace pg {

error::error(std::string msg)
    : msg_{std::move(msg)} {
}

error::error(error const& other) = default;
error& error::operator=(error const& other) = default;
error::error(error&& other) noexcept = default;
error& error::operator=(error&& other) noexcept = default;
error::~error() noexcept = default;
char const* error::what() const noexcept {
    return msg_.data();
}

logic_error::logic_error(std::string msg)
    : error{std::move(msg)} {
}

logic_error::logic_error(logic_error const& other) = default;
logic_error& logic_error::operator=(logic_error const& other) = default;
logic_error::logic_error(logic_error&& other) noexcept = default;
logic_error& logic_error::operator=(logic_error&& other) noexcept = default;
logic_error::~logic_error() noexcept = default;


}  // namespace pg
