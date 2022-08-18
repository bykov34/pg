#pragma once

#include <chrono>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <libpq-fe.h>
#include <pg/internal/bytes.hpp>
#include <pg/error.hpp>
#include <pg/oid.hpp>
#include <pg/time.hpp>

namespace pg {

class field {
public:
    field(field const& other);
    field& operator=(field const& other);
    field(field&& other) noexcept;
    field& operator=(field&& other) noexcept;
    ~field() noexcept;

    template <typename T>
    std::enable_if_t<!std::is_pointer_v<T>, T> as() const {
        T tmp;
        *this >> tmp;
        return tmp;
    }

    template <typename T>
    void operator>>(T*& out) const {
        if (is_null()) {
            out = nullptr;
            return;
        }
        read(*out);
    }

    template <typename T>
    void operator>>(std::optional<T>& out) const {
        if (is_null()) {
            out.reset();
            return;
        }
        out.emplace();
        read(out.value());
    }

    template <typename T>
    void operator>>(T& out) const {
        _POSTGRES_CXX_ASSERT(logic_error,
                             !is_null(),
                             "cannot store NULL value of field '"
                                 << name()
                                 << "' into variable of non-optional type");
        read(out);
    }

    bool is_null() const;
    char const* name() const;
    char const* value() const;
    Oid type() const;
    int length() const;
    int format() const;

private:
    friend class row;

    explicit field(PGresult& res, int row_idx, int col_idx);

    template <typename T>
    std::enable_if_t<std::is_arithmetic_v<T>> read(T& out) const {
        auto const is_ok = [this, &out] {
            switch (type()) {
                case BOOLOID: {
                    return readNum<int8_t>(out);
                }
                case INT2OID: {
                    return readNum<int16_t>(out);
                }
                case DATEOID:
                case INT4OID: {
                    return readNum<int32_t>(out);
                }
                
                case TIMEOID:
                case INT8OID: {
                    return readNum<int64_t>(out);
                }
                case FLOAT4OID: {
                    return readNum<float>(out);
                }
                case FLOAT8OID: {
                    return readNum<double>(out);
                }
                default: {
                    break;
                }
            }
            return false;
        }();
        _POSTGRES_CXX_ASSERT(logic_error,
                             is_ok,
                             "cannot cast field '"
                                 << name()
                                 << "' of type "
                                 << type()
                                 << " to desired arithmetic type");
    };

    template <typename In, typename Out>
    bool readNum(Out& out) const {
        if (std::is_integral_v<In> == std::is_floating_point_v<Out>) {
            return false;
        }

        if (sizeof(Out) < sizeof(In)) {
            return false;
        }

        auto const val = internal::orderBytes<In>(value());
        if (std::is_unsigned_v<Out> && (val < 0)) {
            return false;
        }

        out = static_cast<Out>(val);
        return true;
    }

    void read(time& out) const;
    void read(time::time_point& out) const;
    void read(std::string& out) const;
    void read(std::string_view& out) const;

    PGresult* res_;
    int row_idx_;
    int col_idx_;
};

}  // namespace pg
