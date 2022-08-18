
#include <pg/consumer.hpp>
#include <pg/error.hpp>
#include <pg/status.hpp>

namespace pg {

consumer::consumer(std::shared_ptr<PGconn> handle, int const is_ok)
    : handle_{std::move(handle)}, is_ok_{is_ok == 1} {
    _POSTGRES_CXX_ASSERT(error,
                         isOk(),
                         "fail to send statement: " << PQerrorMessage(handle_.get()));
}

consumer::consumer(consumer&& other) noexcept = default;

consumer& consumer::operator=(consumer&& other) noexcept = default;

consumer::~consumer() noexcept {
    while (!consume().done()) {
    }
}

status consumer::consume() {
    return status{PQgetResult(handle_.get()), this};
}

bool consumer::isOk() const {
    return is_ok_;
}

bool consumer::isBusy() {
    PQconsumeInput(handle_.get());
    return PQisBusy(handle_.get()) == 1;
}

}  // namespace pg
