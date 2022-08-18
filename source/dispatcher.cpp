
#include <pg/internal/dispatcher.hpp>
#include <pg/internal/worker.hpp>
#include <pg/context.hpp>

namespace pg::internal {

dispatcher::dispatcher(std::shared_ptr<context const> ctx, std::shared_ptr<ichannel> chan)
    : ctx_{std::move(ctx)}, chan_{std::move(chan)} {
}

dispatcher::~dispatcher() noexcept {
    switch (ctx_->shutdownPolicy()) {
        case shutdown_policy::DROP: {
            chan_->drop();
            [[fallthrough]];
        }
        case shutdown_policy::GRACEFUL: {
            chan_->quit(size());
            break;
        }
        case shutdown_policy::ABORT: {
            break;
        }
    }
}

void dispatcher::scale(std::tuple<bool, worker*> const params) {
    auto const[is_sent, recycled] = params;
    if (is_sent) {
        return;
    }

    if (recycled != nullptr) {
        recycled->run();
        return;
    }

    if (size() == ctx_->maxConcurrency()) {
        return;
    }

    auto worker = std::make_unique<internal::worker>(ctx_, chan_);
    worker->run();
    workers_.push_back(std::move(worker));
}

inline int dispatcher::size() const {
    return static_cast<int>(workers_.size());
}

}  // namespace pg::internal
