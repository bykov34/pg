#pragma once

#include <functional>
#include <future>
#include <memory>
#include <utility>
#include <vector>
#include <pg/internal/ichannel.hpp>

namespace pg {

class connection;
class context;

}  // namespace postgres

namespace pg::internal {

class worker;

class dispatcher {
public:
    explicit dispatcher(std::shared_ptr<context const> ctx, std::shared_ptr<ichannel> chan);
    dispatcher(dispatcher const& other) = delete;
    dispatcher& operator=(dispatcher const& other) = delete;
    dispatcher(dispatcher&& other) noexcept = delete;
    dispatcher& operator=(dispatcher&& other) noexcept = delete;
    ~dispatcher() noexcept;

    template <typename T>
    std::future<T> send(std::function<T(connection&)> job) {
        auto task = std::make_shared<std::packaged_task<T(connection&)>>(std::move(job));
        scale(chan_->send([task](connection& conn) {
            (*task)(conn);
        }));
        return task->get_future();
    }

private:
    void scale(std::tuple<bool, worker*> params);
    int size() const;

    std::shared_ptr<context const>       ctx_;
    std::shared_ptr<ichannel>            chan_;
    std::vector<std::unique_ptr<worker>> workers_;
};

}  // namespace pg::internal
