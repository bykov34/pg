#pragma once

#include <string>
#include <vector>
#include <libpq-fe.h>

namespace pg {

struct prepared_data {
    std::string      name;
    std::string      statement;
    std::vector<Oid> types;
};

}  // namespace pg
