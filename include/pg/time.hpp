#pragma once

#include <chrono>
#include <string>

namespace pg {

class time
{
    public:
    
        using clock = std::chrono::system_clock;
        using time_point = clock::time_point;

        explicit time();
        explicit time(time_t uni, bool has_zone = false);
        explicit time(time_point pnt, bool has_zone = false);
        explicit time(std::string const& s);
        time(time const& other) noexcept;
        time& operator=(time const& other) noexcept;
        time(time&& other) noexcept;
        time& operator=(time&& other) noexcept;
        ~time() noexcept;

        time_t to_unix() const;
        time_t to_postgres() const;
        std::string to_string() const;
        time_point point() const;
        bool has_zone() const;

        // 2000-01-01 00:00:00
        static auto constexpr EPOCH = time_point{std::chrono::seconds{946684800}};

    private:
    
        time_point pnt_;
        bool  has_zone_ = false;
};

}  // namespace pg
