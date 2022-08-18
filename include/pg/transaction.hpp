#pragma once

namespace pg {

class connection;
class result;

class transaction
{
    public:
    
        transaction(transaction const& other) = delete;
        transaction& operator=(transaction const& other) = delete;
        transaction(transaction&& other) noexcept;
        transaction& operator=(transaction&& other) = delete;
        ~transaction() noexcept;

        result commit();

    private:
    
        friend class connection;
        explicit transaction(connection& conn);
        connection* conn_;
};

}  // namespace pg
