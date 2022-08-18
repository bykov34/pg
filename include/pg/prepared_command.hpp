#pragma once

#include <utility>
#include <pg/command.hpp>

namespace pg {

class prepared_command : public command {
public:
    template <typename... Ts>
    explicit prepared_command(Ts&& ... args)
        : command{std::forward<Ts>(args)...} {
    }

    prepared_command(prepared_command&& other) noexcept = default;
    prepared_command& operator=(prepared_command&& other) noexcept = default;
    ~prepared_command() noexcept = default;
    
    prepared_command(prepared_command const& other) = delete;
    prepared_command& operator=(prepared_command const& other) = delete;
};

}  // namespace pg
