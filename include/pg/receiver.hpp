#pragma once

#include <pg/consumer.hpp>
#include <pg/result.hpp>

namespace pg {

class receiver : public consumer {
public:
    class iterator;

    receiver(receiver const& other) = delete;
    receiver& operator=(receiver const& other) = delete;
    receiver(receiver&& other) noexcept;
    receiver& operator=(receiver&& other) noexcept;
    ~receiver() noexcept;

    result receive();
    iterator begin();
    iterator end();

private:
    friend class connection;

    explicit receiver(std::shared_ptr<PGconn> handle, int is_ok);

    void iter();
};

class receiver::iterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = result;
    using pointer = result*;
    using reference = result&;

    iterator(iterator const& other) = delete;
    iterator& operator=(iterator const& other) = delete;
    iterator(iterator&& other) noexcept;
    iterator& operator=(iterator&& other) noexcept;
    ~iterator() noexcept;

    bool operator==(iterator const& other) const;
    bool operator!=(iterator const& other) const;
    void operator++();
    iterator const operator++(int);
    result operator->();
    result operator*();

private:
    friend class receiver;

    explicit iterator(receiver& rec, result res);

    receiver* rec_;
    result res_;
    bool   is_;
};

}  // namespace pg
