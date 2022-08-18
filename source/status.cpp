
#include <pg/status.hpp>
#include <pg/error.hpp>

namespace pg {

status::status(PGresult* const handle)
    : handle_{handle, PQclear} {
    check();
}

status::status(PGresult* const handle, pg::consumer*)
    : handle_{handle, PQclear} {
    // Null result is valid in asynchronous mode.
    // It indicates an end of rows stream.
    if (handle) {
        check();
    }
}

status::status(status&& other) noexcept = default;

status& status::operator=(status&& other) noexcept = default;

status::~status() noexcept = default;

void status::check() const {
    if (!handle_) {
        _POSTGRES_CXX_FAIL(logic_error, "rows stream is over");
    }

    _POSTGRES_CXX_ASSERT(error,
                         ok(),
                         "fail to execute operation: " << describe() << ": " << message());
}

bool status::ok() const {
    switch (type()) {
        case PGRES_COMMAND_OK:
        case PGRES_TUPLES_OK:
        case PGRES_SINGLE_TUPLE:
        case PGRES_NONFATAL_ERROR: {
            return true;
        }
        default: {
            break;
        }
    }
    return false;
}

bool status::done() const {
    return native() == nullptr;
}

bool status::empty() const {
    return size() == 0;
}

int status::size() const {
    return PQntuples(native());
}

int status::effect() const {
    std::string const s = PQcmdTuples(native());
    return s.empty() ? 0 : std::stoi(s);
}

const char* status::message() const {
    return PQresultErrorMessage(native());
}

const char* status::describe() const {
    return PQresStatus(type());
}

ExecStatusType status::type() const {
    return PQresultStatus(native());
}

PGresult* status::native() const {
    return handle_.get();
}

}  // namespace pg
