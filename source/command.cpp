#include <pg/command.hpp>
#include <cstring>

namespace pg {

command::command(command&& other) noexcept = default;

command& command::operator=(command&& other) noexcept = default;

command::~command() noexcept = default;

char const* command::statement() const {
    return stmt_;
}

int command::count() const {
    return static_cast<int>(values_.size());
}

Oid const* command::types() const {
    return types_.data();
}

char const* const* command::values() const {
    return values_.data();
}

int const* command::lengths() const {
    return lengths_.data();
}

int const* command::formats() const {
    return formats_.data();
}

void command::add(std::nullptr_t) {
    setMeta(0, 0, 0);
    values_.push_back(nullptr);
}

void command::add(std::chrono::system_clock::time_point const t) {
    add(time{t});
}

void command::add(time const& t) {
    if (t.has_zone()) {
        add(t.to_string());
        types_.back() = TIMESTAMPTZOID;
        return;
    }

    add(t.to_postgres());
    types_.back() = TIMESTAMPOID;
}

void command::add(std::string const& s) {
    addText(s.data(), s.size() + 1);
}

void command::add(std::string_view const s) {
    add(s.data());
}

void command::add(char const* const s) {
    setMeta(0, 0, 0);
    values_.push_back(s);
}

void command::addText(char const* const s, size_t const len) {
    setMeta(0, static_cast<int>(len), 0);
    storeData(s, len);
}

void command::setMeta(Oid const id, int const len, int const fmt) {
    types_.push_back(id);
    lengths_.push_back(len);
    formats_.push_back(fmt);
}

void command::storeData(void const* const arg, size_t const len) {
    auto       storage = buf_.data();
    auto const old_len = buf_.size();
    auto const new_len = old_len + len;
    buf_.resize(new_len);
    if (buf_.data() != storage) {
        storage = buf_.data();
        for (auto i = 0u; i < values_.size(); ++i) {
            if (values_[i] && lengths_[i]) {
                values_[i] = storage;
                storage += lengths_[i];
            }
        }
    } else {
        storage += old_len;
    }
    memcpy(storage, arg, len);
    values_.push_back(storage);
}

void command::setStatement(std::string stmt) {
    stmt_buf_ = std::move(stmt);
    stmt_ = stmt_buf_.data();
}

void command::setStatement(std::string_view const stmt) {
    stmt_ = stmt.data();
}

void command::setStatement(char const* const stmt) {
    stmt_ = stmt;
}

}  // namespace pg
