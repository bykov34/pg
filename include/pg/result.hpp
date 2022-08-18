#pragma once

#include <pg/status.hpp>

namespace pg {

class row;

class result : public status {
public:
    class iterator;

    result(result const& other) = delete;
    result& operator=(result const& other) = delete;
    result(result&& other) noexcept;
    result& operator=(result&& other) noexcept;
    ~result() noexcept;

    iterator begin() const;
    iterator end() const;
    row operator[](int idx) const;

private:
    friend class connection;
    friend class receiver;

    explicit result(PGresult* handle);
    explicit result(PGresult* handle, consumer* consumer);
};

class result::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = row;
    using pointer = row*;
    using reference = row&;

    iterator(iterator const& other);
    iterator& operator=(iterator const& other);
    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;
    ~iterator() noexcept;

    bool operator==(iterator const& other) const;
    bool operator!=(iterator const& other) const;
    void operator++();
    iterator const operator++(int);
    row operator->() const;
    row operator*() const;

private:
    friend class result;

    explicit iterator(PGresult& handle, int idx);

    PGresult* handle_ = nullptr;
    int idx_ = 0;
};

}  // namespace pg
