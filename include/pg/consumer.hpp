#pragma once

#include <memory>
#include <libpq-fe.h>

namespace pg {

class status;

class consumer {
public:
    consumer(consumer const& other) = delete;
    consumer& operator=(consumer const& other) = delete;
    consumer(consumer&& other) noexcept;
    consumer& operator=(consumer&& other) noexcept;
    ~consumer() noexcept;

    status consume();
    bool isOk() const;
    bool isBusy();

protected:
    friend class connection;

    explicit consumer(std::shared_ptr<PGconn> handle, int is_ok);

    std::shared_ptr<PGconn> handle_;
    bool                    is_ok_ = false;
};

}  // namespace pg
