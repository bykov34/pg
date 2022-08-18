#include <pg/internal/worker.hpp>

#include <utility>
#include <pg/internal/ichannel.hpp>
#include <pg/connection.hpp>
#include <pg/context.hpp>

namespace pg::internal {

worker::worker(std::shared_ptr<context const> ctx, std::shared_ptr<ichannel> chan)
    : ctx_{std::move(ctx)}, chan_{std::move(chan)} {
}

worker::~worker() noexcept {
    if (thread_.joinable()) {
        switch (ctx_->shutdownPolicy()) {
            case shutdown_policy::GRACEFUL:
            case shutdown_policy::DROP: {
                thread_.join();
                break;
            }
            case shutdown_policy::ABORT: {
                thread_.detach();
                break;
            }
        }
    }
}

void worker::run() {
    if (thread_.joinable()) {
        thread_.join();
    }
    thread_ = std::thread([this, conn = ctx_->connect()]() mutable {
        while (true) {
            chan_->receive(slot_);
            auto const job = std::move(slot_.job_);
            if (!job) {
                break;
            }

            job(conn);
            if (!conn.isOk()) {
                break;
            }
        }
        chan_->recycle(*this);
    });
}

}  // namespace pg::internal
