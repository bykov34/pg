
#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <pg/config.hpp>
#include <pg/prepare_data.hpp>

namespace pg {

class connection;

enum class shutdown_policy {
    GRACEFUL,
    DROP,
    ABORT,
};

class context {
public:
    class builder;
    using duration = std::chrono::high_resolution_clock::duration;

    explicit context();
    context(context const& other) = delete;
    context& operator=(context const& other) = delete;
    context(context&& other) noexcept;
    context& operator=(context&& other) noexcept;
    ~context() noexcept;

    connection connect() const;
    duration idleTimeout() const;
    int maxConcurrency() const;
    int maxQueueSize() const;
    shutdown_policy shutdownPolicy() const;

private:
    config                   cfg_;
    std::string              uri_;
    std::vector<prepared_data> preparings_;
    duration                 max_idle_;
    int                      max_concur_;
    int                      max_queue_;
    shutdown_policy           shut_pol_;
};

class context::builder {
public:
    explicit builder();
    builder(builder const& other) = delete;
    builder& operator=(builder const& other) = delete;
    builder(builder&& other) noexcept;
    builder& operator=(builder&& other) noexcept;
    ~builder() noexcept;

    builder& config(config cfg);
    builder& uri(std::string uri);
    builder& prepare(prepared_data prep);
    builder& idleTimeout(context::duration val);
    builder& maxConcurrency(int val);
    builder& maxQueueSize(int val);
    builder& shutdownPolicy(shutdown_policy val);

    context build();
    std::shared_ptr<context> share();

private:
    context ctx_;
};

}  // namespace pg
