#pragma once

#include <string>
#include <type_traits>
#include <libpq-fe.h>
#include <pg/internal/classifier.hpp>
#include <pg/field.hpp>

namespace pg {

class row
{
    public:
        
        class iterator;
        
        row(row const& other);
        row& operator=(row const& other);
        row(row&& other) noexcept;
        row& operator=(row&& other) noexcept;
        ~row() noexcept;

        template <typename T>
        std::enable_if_t<internal::isVisitable<T>(), row&> operator>>(T& val) {
            val.visitPostgresFields(*this);
            return *this;
        };

        template <typename T>
        std::enable_if_t<!internal::isVisitable<T>(), row&> operator>>(T& val) {
            (*this)[col_idx_++] >> val;
            return *this;
        }

        template <typename T>
        void accept(char const* const name, T& val) {
            (*this)[name] >> val;
        };

        field operator[](std::string const& col_name) const;
        field operator[](char const* col_name) const;
        field operator[](int col_idx) const;

        iterator begin() const;
        iterator end() const;
        
        int size() const;

    private:
    
        friend class result;

        explicit row(PGresult& res, int row_idx);

        PGresult* res_;
        int row_idx_;
        int col_idx_;
};

class row::iterator
{
    public:
    
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = field;
        using pointer = field*;
        using reference = field&;

        iterator(iterator const& other);
        iterator& operator=(iterator const& other);
        iterator(iterator&& other) noexcept;
        iterator& operator=(iterator&& other) noexcept;
        ~iterator() noexcept;

        bool operator==(iterator const& other) const;
        bool operator!=(iterator const& other) const;
        void operator++();
        iterator const operator++(int);
        value_type operator->() const;
        value_type operator*() const;

    private:
    
        friend class row;

        explicit iterator(PGresult& handle, int row_idx, int col_idx);

        PGresult* handle_ = nullptr;
        int row_idx_ = 0;
        int col_idx_ = 0;
};

}  // namespace pg
