
#include <pg/transaction.hpp>
#include <pg/connection.hpp>
#include <pg/error.hpp>

namespace pg {

transaction::transaction(connection& conn)
    : conn_{&conn} {
}

transaction::transaction(transaction&& other) noexcept
    : conn_{other.conn_} {
    other.conn_ = nullptr;
}

transaction::~transaction() noexcept {
    if (conn_) {
        conn_->exec("ROLLBACK");
    }
}

result transaction::commit() {
    _POSTGRES_CXX_ASSERT(logic_error, conn_, "no transaction in progress");
    auto const conn = conn_;
    conn_ = nullptr;
    return conn->exec("COMMIT");
}

}  // namespace pg
