#include <pg/row.hpp>
#include <pg/error.hpp>

namespace pg {

row::row(PGresult& res, int const row_idx)
    : res_{&res}, row_idx_{row_idx}, col_idx_{0} {
}

row::row(row const& other) = default;
row& row::operator=(row const& other) = default;
row::row(row&& other) noexcept = default;
row& row::operator=(row&& other) noexcept = default;
row::~row() noexcept = default;

field row::operator[](std::string const& col_name) const {
    return (*this)[col_name.c_str()];
}

field row::operator[](char const* const col_name) const {
    auto const col_idx = PQfnumber(res_, col_name);
    _POSTGRES_CXX_ASSERT(logic_error, (0 <= col_idx), "column '" << col_name << "' does not exist");
    return field{*res_, row_idx_, col_idx};
}

field row::operator[](int const col_idx) const {
    _POSTGRES_CXX_ASSERT(logic_error,
                         (0 <= col_idx) && (col_idx < size()),
                         "column index " << col_idx << " is out of range");
    return field{*res_, row_idx_, col_idx};
}

row::iterator row::begin() const {
    return iterator{*res_, row_idx_, 0};
}

row::iterator row::end() const {
    return iterator{*res_, row_idx_, size()};
}

int row::size() const {
    return PQnfields(res_);
}

row::iterator::iterator(PGresult& handle, int row_idx, int const col_idx):
                        handle_{&handle}, row_idx_{row_idx}, col_idx_{col_idx}
{
}

row::iterator::iterator(iterator const& other) = default;
row::iterator& row::iterator::operator=(iterator const& other) = default;
row::iterator::iterator(iterator&& other) noexcept = default;
row::iterator& row::iterator::operator=(iterator&& other) noexcept = default;
row::iterator::~iterator() noexcept = default;

bool row::iterator::operator==(iterator const& other) const {
    return (handle_ == other.handle_) && (row_idx_ == other.row_idx_) && (col_idx_ == other.col_idx_);
}

bool row::iterator::operator!=(iterator const& other) const {
    return !(*this == other);
}

void row::iterator::operator++() {
    ++col_idx_;
}

row::iterator const row::iterator::operator++(int) {
    return row::iterator{*handle_, row_idx_, col_idx_++};
}

field row::iterator::operator->() const {
    return this->operator*();
}

field row::iterator::operator*() const {
    _POSTGRES_CXX_ASSERT(logic_error,
                         (0 <= col_idx_) && (col_idx_ < PQnfields(handle_)),
                         "column index " << col_idx_ << " is out of range");
    return field{*handle_, row_idx_, col_idx_};
}

}  // namespace pg
