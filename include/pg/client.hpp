#pragma once

#include <functional>
#include <future>
#include <memory>

namespace pg::internal {

class dispatcher;

}  // namespace pg::internal

namespace pg {

class connection;
class context;
class result;
class status;

class client
{
    public:
    
        explicit client();
        explicit client(context ctx);
        client(client const& other) = delete;
        client& operator=(client const& other) = delete;
        client(client&& other) noexcept;
        client& operator=(client&& other) noexcept;
        ~client() noexcept;

        std::future<status> exec(std::function<status(connection&)> job);
        std::future<result> query(std::function<result(connection&)> job);

    private:
        
        using impl = internal::dispatcher;
        std::unique_ptr<impl> impl_;
};

}  // namespace pg
