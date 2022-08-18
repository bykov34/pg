
#include <utility>

#include <pg/internal/channel.hpp>
#include <pg/context.hpp>
#include <pg/error.hpp>

namespace pg::internal {

channel::channel(std::shared_ptr<context const> ctx)
    : ctx_{std::move(ctx)} {
}

channel::~channel() noexcept = default;

void channel::quit(int count) {
    while (0 < count--) {
        send(nullptr, 0);
    }
}

std::tuple<bool, worker*> channel::send(job job) {
    return send(std::move(job), ctx_->maxQueueSize());
}

std::tuple<bool, worker*> channel::send(job _job, int const lim) {
    std::unique_lock c_guard{mtx_};
    if (!slots_.empty()) {
        auto const it   = slots_.begin();
        auto const slot = *it;
        slots_.erase(it);
        c_guard.unlock();

        std::lock_guard s_guard{slot->mtx};
        slot->job_.swap(_job);
        slot->signal.notify_one();
        return {true, nullptr};
    }

    if (0 < lim) {
        _POSTGRES_CXX_ASSERT(error,
                             (static_cast<int>(queue_.size()) < lim),
                             "queue overflow");
    }

    queue_.push(std::move(_job));
    if (recreation_.empty()) {
        return {false, nullptr};
    }

    auto const worker = recreation_.back();
    recreation_.pop_back();
    return {false, worker};
}

void channel::receive(slot& slot) {
    std::unique_lock c_guard{mtx_};
    if (!queue_.empty()) {
        queue_.front().swap(slot.job_);
        queue_.pop();
        return;
    }

    // Keep slots sorted to detect idle workers.
    slots_.insert(&slot);
    // Prevent filling the slot until waiting.
    std::unique_lock s_guard{slot.mtx};
    c_guard.unlock();

    auto const timeout = ctx_->idleTimeout();
    if (timeout.count() == 0) {
        slot.signal.wait(s_guard);
        return;
    }

    if (slot.signal.wait_for(s_guard, timeout) == std::cv_status::no_timeout) {
        return;
    }

    // Check if other thread is going to fill the slot.
    c_guard.lock();
    if (0 < slots_.erase(&slot)) {
        return;
    }

    c_guard.unlock();
    slot.signal.wait(s_guard);
}

void channel::recycle(worker& worker) {
    std::lock_guard guard{mtx_};
    recreation_.push_back(&worker);
}

void channel::drop() {
    std::lock_guard guard{mtx_};
    auto const      garbage = std::move(queue_);
}

}  // namespace pg::internal
