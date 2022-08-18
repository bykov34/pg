#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>

namespace pg {

class connection;

}  // namespace postgres

namespace pg::internal {

using job = std::function<void(connection&)>;

struct slot {
    job                     job_;
    std::condition_variable signal;
    std::mutex              mtx;
};

}  // namespace pg::internal
