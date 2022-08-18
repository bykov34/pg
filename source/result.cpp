
#include <pg/result.hpp>
#include <pg/error.hpp>
#include <pg/row.hpp>

namespace pg {

result::result(PGresult* const handle)
    : status{handle} {
}

result::result(PGresult* const handle, pg::consumer* const consumer)
    : status{handle, consumer} {
}

result::result(result&& other) noexcept = default;
result& result::operator=(result&& other) noexcept = default;
result::~result() noexcept = default;

result::iterator result::begin() const {
    check();
    return iterator{*native(), 0};
}

result::iterator result::end() const {
    check();
    return iterator{*native(), size()};
}

row result::operator[](int const idx) const {
    check();
    return *iterator{*native(), idx};
}

result::iterator::iterator(PGresult& handle, int const idx)
    : handle_{&handle}, idx_{idx} {
}

result::iterator::iterator(iterator const& other) = default;
result::iterator& result::iterator::operator=(iterator const& other) = default;
result::iterator::iterator(iterator&& other) noexcept = default;
result::iterator& result::iterator::operator=(iterator&& other) noexcept = default;
result::iterator::~iterator() noexcept = default;

bool result::iterator::operator==(iterator const& other) const {
    return (handle_ == other.handle_) && (idx_ == other.idx_);
}

bool result::iterator::operator!=(iterator const& other) const {
    return !(*this == other);
}

void result::iterator::operator++() {
    ++idx_;
}

result::iterator const result::iterator::operator++(int) {
    return result::iterator{*handle_, idx_++};
}

row result::iterator::operator->() const {
    return this->operator*();
}

row result::iterator::operator*() const {
    _POSTGRES_CXX_ASSERT(logic_error,
                         (0 <= idx_) && (idx_ < PQntuples(handle_)),
                         "row index " << idx_ << " is out of range");
    return row{*handle_, idx_};
}

}  // namespace pg
