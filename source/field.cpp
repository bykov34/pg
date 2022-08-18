#include <pg/field.hpp>

namespace pg {

field::field(PGresult& res, int const row_idx, int const col_idx)
    : res_{&res}, row_idx_{row_idx}, col_idx_{col_idx} {
}

field::field(field const& other) = default;

field& field::operator=(field const& other) = default;

field::field(field&& other) noexcept = default;

field& field::operator=(field&& other) noexcept = default;

field::~field() noexcept = default;

void field::read(time& out) const {
    time::time_point pnt{};
    read(pnt);
    out = time{pnt};
}

void field::read(time::time_point& out) const {
    _POSTGRES_CXX_ASSERT(logic_error,
                         type() == TIMESTAMPOID,
                         "cannot cast field '"
                             << name()
                             << "' of type "
                             << type()
                             << " to timestamp without time zone");

    out = time::EPOCH;
    out += std::chrono::microseconds{internal::orderBytes<int64_t>(value())};
}

void field::read(std::string& out) const {
    out = std::string{value(), static_cast<size_t>(length())};
}

void field::read(std::string_view& out) const {
    out = std::string_view{value(), static_cast<size_t>(length())};
}

bool field::is_null() const {
    return PQgetisnull(res_, row_idx_, col_idx_) == 1;
}

char const* field::name() const {
    return PQfname(res_, col_idx_);
}

char const* field::value() const {
    return PQgetvalue(res_, row_idx_, col_idx_);
}

Oid field::type() const {
    return PQftype(res_, col_idx_);
}

int field::length() const {
    return PQgetlength(res_, row_idx_, col_idx_);
}

int field::format() const {
    return PQfformat(res_, col_idx_);
}

}  // namespace pg
