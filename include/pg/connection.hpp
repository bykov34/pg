#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <libpq-fe.h>
#include <pg/command.hpp>
#include <pg/result.hpp>
#include <pg/row.hpp>
#include <pg/statement.hpp>
#include <pg/transaction.hpp>

namespace pg {

class config;
class consumer;
class prepared_command;
class receiver;
struct prepared_data;

class connection {
public:
    static PGPing ping();
    static PGPing ping(config const& cfg);
    static PGPing ping(std::string const& uri);

    explicit connection();
    explicit connection(config const& cfg);
    explicit connection(std::string const& uri);
    connection(connection const& other) = delete;
    connection& operator=(connection const& other) = delete;
    connection(connection&& other) noexcept;
    connection& operator=(connection&& other) noexcept;
    ~connection() noexcept;

    template <typename T>
    status create() {
        return exec(statement<T>::create());
    }

    template <typename T>
    status drop() {
        return exec(statement<T>::drop());
    }

    template <typename T>
    status insert(T const& val) {
        return exec(command{statement<T>::insert(), val});
    }

    template <typename Iter>
    status insert(Iter const it, Iter const end) {
        return exec(command{RangeStatement::insert(it, end), std::make_pair(it, end)});
    }

    template <typename T>
    status update(T const& val) {
        return exec(command{statement<T>::update(), val});
    }

    template <typename T>
    result select(std::vector<T>& out) {
        auto res = exec(statement<T>::select());
        if (!res.isOk()) {
            return res;
        }

        out.reserve(out.size() + res.size());
        for (auto row : res) {
            out.emplace_back();
            row >> out.back();
        }
        return res;
    }

    template <typename... Ts>
    std::enable_if_t<(1 < sizeof... (Ts)), result> transact(Ts&& ... args) {
        auto tx  = begin();
        auto res = exec(std::forward<Ts>(args)...);
        tx.commit();
        return res;
    }

    result exec(prepared_data const& prep);
    result exec(command const& cmd);
    result exec(prepared_command const& cmd);
    status execRaw(std::string_view stmt);

    receiver send(prepared_data const& prep);
    receiver send(command const& cmd);
    receiver send(prepared_command const& cmd);
    consumer sendRaw(std::string_view stmt);

    receiver iter(command const& cmd);
    receiver iter(prepared_command const& cmd);

    transaction begin();

    bool reset();
    bool isOk();
    std::string message();

    std::string esc(std::string const& in);
    std::string escId(std::string const& in);

    PGconn* native() const;
    
    std::vector<std::string> check_notifications();

private:
    explicit connection(PGconn* handle);

    template <typename T, typename... Ts>
    std::enable_if_t<(0 < sizeof... (Ts)), result> exec(T&& arg, Ts&& ... args) {
        auto res = exec(std::forward<T>(arg));
        if (!res.ok()) {
            return res;
        }

        return exec(std::forward<Ts>(args)...);
    };

    template <typename F>
    std::string doEsc(std::string const& in, F f);

    std::shared_ptr<PGconn> handle_;
};

}  // namespace pg
