#include <pg/client.hpp>

#include <utility>
#include <pg/internal/channel.hpp>
#include <pg/internal/dispatcher.hpp>
#include <pg/context.hpp>
#include <pg/result.hpp>
#include <pg/status.hpp>

namespace pg {

client::client()
    : client{context{}} {
}

client::client(context ctx) {
    auto pctx = std::make_shared<context>(std::move(ctx));
    auto chan = std::make_shared<internal::channel>(pctx);
    impl_ = std::make_unique<impl>(std::move(pctx), std::move(chan));
}

client::client(client&& other) noexcept = default;

client& client::operator=(client&& other) noexcept = default;

client::~client() noexcept = default;

std::future<status> client::exec(std::function<status(connection&)> job) {
    return impl_->send(std::move(job));
}

std::future<result> client::query(std::function<result(connection&)> job) {
    return impl_->send(std::move(job));
}

}  // namespace pg
