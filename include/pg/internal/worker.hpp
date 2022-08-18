#pragma once

#include <memory>
#include <thread>
#include <pg/internal/job.hpp>

namespace pg {

class context;

}  // namespace pg

namespace pg::internal {

class ichannel;

class worker {
public:
    explicit worker(std::shared_ptr<context const> ctx, std::shared_ptr<ichannel> chan);
    worker(worker const& other) = delete;
    worker& operator=(worker const& other) = delete;
    worker(worker&& other) noexcept = delete;
    worker& operator=(worker&& other) noexcept = delete;
    ~worker() noexcept;

    void run();

private:
    std::shared_ptr<context const> ctx_;
    std::shared_ptr<ichannel>      chan_;
    slot                           slot_;
    std::thread                    thread_;
};

}  // namespace pg::internal
