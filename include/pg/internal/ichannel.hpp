#pragma once

#include <tuple>
#include <pg/internal/job.hpp>

namespace pg::internal {

class worker;

class ichannel {
public:
    virtual ~ichannel() noexcept;

    virtual void quit(int count) = 0;
    virtual std::tuple<bool, worker*> send(job job) = 0;
    virtual void receive(slot& slot) = 0;
    virtual void recycle(worker& w) = 0;
    virtual void drop() = 0;
};

}  // namespace pg::internal
