
#include <pg/connection.hpp>
#include <pg/config.hpp>
#include <pg/consumer.hpp>
#include <pg/error.hpp>
#include <pg/prepared_command.hpp>
#include <pg/prepare_data.hpp>
#include <pg/receiver.hpp>

namespace pg {

enum {
    EXPAND_DBNAME = 0,
    RESULT_FORMAT = 1,
};

PGPing connection::ping() {
    return ping(config::build());
}

PGPing connection::ping(config const& cfg) {
    return PQpingParams(cfg.keys(), cfg.values(), EXPAND_DBNAME);
}

PGPing connection::ping(std::string const& uri) {
    return PQping(uri.data());
}

connection::connection()
    : connection{config::build()} {
}

connection::connection(config const& cfg)
    : connection{PQconnectdbParams(cfg.keys(), cfg.values(), EXPAND_DBNAME)} {
}

connection::connection(std::string const& uri)
    : connection{PQconnectdb(uri.data())} {
}

connection::connection(PGconn* const handle)
    : handle_{handle, PQfinish} {
    _POSTGRES_CXX_ASSERT(error, isOk(), "fail to connect: " << message());
}

connection::connection(connection&& other) noexcept = default;

connection& connection::operator=(connection&& other) noexcept = default;

connection::~connection() noexcept = default;

result connection::exec(prepared_data const& prep) {
    return result{PQprepare(native(),
                            prep.name.data(),
                            prep.statement.data(),
                            static_cast<int>(prep.types.size()),
                            prep.types.data())};
}

result connection::exec(command const& cmd) {
    return result{PQexecParams(native(),
                               cmd.statement(),
                               cmd.count(),
                               cmd.types(),
                               cmd.values(),
                               cmd.lengths(),
                               cmd.formats(),
                               RESULT_FORMAT)};
}

result connection::exec(prepared_command const& cmd) {
    return result{PQexecPrepared(native(),
                                 cmd.statement(),
                                 cmd.count(),
                                 cmd.values(),
                                 cmd.lengths(),
                                 cmd.formats(),
                                 RESULT_FORMAT)};
}

status connection::execRaw(std::string_view const stmt) {
    return status{PQexec(native(), stmt.data())};
}

receiver connection::send(prepared_data const& prep) {
    return receiver{handle_,
                    PQsendPrepare(native(),
                                  prep.name.data(),
                                  prep.statement.data(),
                                  static_cast<int>(prep.types.size()),
                                  prep.types.data())};
}

receiver connection::send(command const& cmd) {
    return receiver{handle_,
                    PQsendQueryParams(native(),
                                      cmd.statement(),
                                      cmd.count(),
                                      cmd.types(),
                                      cmd.values(),
                                      cmd.lengths(),
                                      cmd.formats(),
                                      RESULT_FORMAT)};
}

receiver connection::send(prepared_command const& cmd) {
    return receiver{handle_,
                    PQsendQueryPrepared(native(),
                                        cmd.statement(),
                                        cmd.count(),
                                        cmd.values(),
                                        cmd.lengths(),
                                        cmd.formats(),
                                        RESULT_FORMAT)};
}

consumer connection::sendRaw(std::string_view const stmt) {
    return consumer{handle_, PQsendQuery(native(), stmt.data())};
}

receiver connection::iter(command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
    return rcvr;
}

receiver connection::iter(prepared_command const& cmd) {
    auto rcvr = send(cmd);
    rcvr.iter();
    return rcvr;
}

transaction connection::begin() {
    exec("BEGIN");
    return transaction{*this};
}

bool connection::reset() {
    PQreset(native());
    return isOk();
}

bool connection::isOk() {
    return PQstatus(native()) == CONNECTION_OK;
}

std::string connection::message() {
    return PQerrorMessage(native());
}

std::string connection::esc(std::string const& in) {
    return doEsc(in, PQescapeLiteral);
}

std::string connection::escId(std::string const& in) {
    return doEsc(in, PQescapeIdentifier);
}

template <typename F>
std::string connection::doEsc(std::string const& in, F const f) {
    auto const escaped = f(native(), in.data(), in.size());
    _POSTGRES_CXX_ASSERT(error, (escaped != nullptr), "fail to escape: " << message());
    std::string res = escaped;
    PQfreemem(escaped);
    return res;
}

PGconn* connection::native() const {
    return handle_.get();
}

std::vector<std::string> connection::check_notifications()
{
    auto conn = native();
    auto res = PQconsumeInput(conn);
    _POSTGRES_CXX_ASSERT(error, (res == 1), "failed to consume input: " << message());
    
    std::vector<std::string> result;
    auto* notification = PQnotifies(conn);
    while (notification != nullptr)
    {
        result.push_back(notification->relname);
        PQfreemem(notification);
        notification = PQnotifies(conn);
    }
    
    return result;
}

}  // namespace pg
