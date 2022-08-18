
#include <thread>

#include <pg/context.hpp>
#include <pg/connection.hpp>
#include <pg/error.hpp>

namespace pg {

context::context()
    : cfg_{config::build()},
      max_idle_{0},
      max_concur_{static_cast<int>(std::thread::hardware_concurrency())},
      max_queue_{0},
      shut_pol_{shutdown_policy::GRACEFUL} {
}

context::context(context&& other) noexcept = default;

context& context::operator=(context&& other) noexcept = default;

context::~context() noexcept = default;

connection context::connect() const {
    auto conn = uri_.empty() ? connection{cfg_} : connection{uri_};
    for (auto const& prep : preparings_) {
        conn.exec(prep);
    }
    return conn;
}

context::duration context::idleTimeout() const {
    return max_idle_;
}

int context::maxConcurrency() const {
    return max_concur_;
}

int context::maxQueueSize() const {
    return max_queue_;
}

shutdown_policy context::shutdownPolicy() const {
    return shut_pol_;
}

context::builder::builder() = default;

context::builder::builder(context::builder&& other) noexcept = default;

context::builder& context::builder::operator=(context::builder&& other) noexcept = default;

context::builder::~builder() noexcept = default;

context::builder& context::builder::config(class config cfg) {
    ctx_.cfg_ = std::move(cfg);
    return *this;
}

context::builder& context::builder::uri(std::string uri) {
    ctx_.uri_ = std::move(uri);
    return *this;
}

context::builder& context::builder::prepare(prepared_data prep) {
    ctx_.preparings_.push_back(std::move(prep));
    return *this;
}

context::builder& context::builder::idleTimeout(context::duration const val) {
    _POSTGRES_CXX_ASSERT(logic_error, 0 <= val.count(), "bad idle timeout: " << val.count());
    ctx_.max_idle_ = val;
    return *this;
}

context::builder& context::builder::maxConcurrency(int const val) {
    _POSTGRES_CXX_ASSERT(logic_error, 1 <= val, "bad concurrency: " << val);
    ctx_.max_concur_ = val;
    return *this;
}

context::builder& context::builder::maxQueueSize(int const val) {
    _POSTGRES_CXX_ASSERT(logic_error, 0 <= val, "bad queue size: " << val);
    ctx_.max_queue_ = val;
    return *this;
}

context::builder& context::builder::shutdownPolicy(shutdown_policy const val) {
    ctx_.shut_pol_ = val;
    return *this;
}

context context::builder::build() {
    return std::move(ctx_);
}

std::shared_ptr<context> context::builder::share() {
    return std::make_shared<context>(build());
}

}  // namespace pg
