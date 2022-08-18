
#include <pg/receiver.hpp>

namespace pg {

receiver::receiver(std::shared_ptr<PGconn> handle, int const is_ok)
    : consumer{std::move(handle), is_ok} {
}

receiver::receiver(receiver&& other) noexcept = default;

receiver& receiver::operator=(receiver&& other) noexcept = default;

receiver::~receiver() noexcept = default;

result receiver::receive() {
    return result{PQgetResult(handle_.get()), this};
}

void receiver::iter() {
    is_ok_ = is_ok_ && (PQsetSingleRowMode(handle_.get()) == 1);
}

receiver::iterator receiver::begin() {
    return iterator{*this, receive()};
}

receiver::iterator receiver::end() {
    return iterator{*this, result{nullptr, this}};
}

receiver::iterator::iterator(receiver& rec, result res)
    : rec_{&rec}, res_{std::move(res)}, is_{!res_.done()} {
}

receiver::iterator::iterator(iterator&& other) noexcept = default;

receiver::iterator& receiver::iterator::operator=(iterator&& other) noexcept = default;

receiver::iterator::~iterator() noexcept = default;

bool receiver::iterator::operator==(iterator const& other) const {
    return (rec_ == other.rec_) && (is_ == other.is_);
}

bool receiver::iterator::operator!=(iterator const& other) const {
    return !(*this == other);
}

void receiver::iterator::operator++() {
    *this = rec_->begin();
}

receiver::iterator const receiver::iterator::operator++(int) {
    return rec_->begin();
}

result receiver::iterator::operator->() {
    return this->operator*();
}

result receiver::iterator::operator*() {
    return std::move(res_);
}

}  // namespace pg
