#pragma once

#include <sstream>
#include <stdexcept>
#include <string>

namespace pg {

class error : public std::exception
{
    public:
    
        explicit error(std::string msg);
        error(error const& other);
        error& operator=(error const& other);
        error(error&& other) noexcept;
        error& operator=(error&& other) noexcept;
        ~error() noexcept override;

        char const* what() const noexcept override;

    private:
    
        std::string msg_;
};

class logic_error : public error
{
    public:
    
        explicit logic_error(std::string msg);
        logic_error(logic_error const& other);
        logic_error& operator=(logic_error const& other);
        logic_error(logic_error&& other) noexcept;
        logic_error& operator=(logic_error&& other) noexcept;
        ~logic_error() noexcept override;
};

}  // namespace pg

#define _POSTGRES_CXX_FAIL(T, msg) \
    { \
        std::stringstream stream{}; \
        stream << "PostgreSQL client error: " << msg; \
        throw pg::T{stream.str()}; \
    }

#define _POSTGRES_CXX_ASSERT(T, cond, msg) \
    if (!(cond)) { \
        _POSTGRES_CXX_FAIL(T, msg); \
    }
