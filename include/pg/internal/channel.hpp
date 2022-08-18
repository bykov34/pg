#pragma once

#include <memory>
#include <mutex>
#include <queue>
#include <set>
#include <vector>
#include <pg/internal/ichannel.hpp>

namespace pg {

class context;

}  // namespace postgres

namespace pg::internal {

class channel : public ichannel {
public:
    explicit channel(std::shared_ptr<context const> ctx);
    channel(channel const& other) = delete;
    channel& operator=(channel const& other) = delete;
    channel(channel&& other) noexcept = delete;
    channel& operator=(channel&& other) noexcept = delete;
    ~channel() noexcept override;

    std::tuple<bool, worker*> send(job job) override;
    void receive(slot& slot) override;
    void recycle(worker& worker) override;
    void drop() override;
    void quit(int count) override;

private:
    std::tuple<bool, worker*> send(job job, int lim);

    std::shared_ptr<context const> ctx_;
    std::queue<job>                queue_;
    std::set<slot*>                slots_;
    std::vector<worker*>           recreation_;
    std::mutex                     mtx_;
};

}  // namespace pg::internal
