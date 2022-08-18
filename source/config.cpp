#include <pg/config.hpp>

namespace pg {

config::config() = default;

config::config(config&& other) noexcept = default;

config& config::operator=(config&& other) noexcept = default;

config::~config() noexcept = default;

config config::build() {
    return builder{}.build();
}

char const* const* config::keys() const {
    return keys_.data();
}

char const* const* config::values() const {
    return vals_.data();
}

config::builder::builder() = default;

config::builder::builder(builder&& other) noexcept = default;

config::builder& config::builder::operator=(builder&& other) noexcept = default;

config::builder::~builder() noexcept = default;

config::builder& config::builder::application_name(std::string const& val) {
    return set("application_name", val);
}

config::builder& config::builder::client_encoding(std::string const& val) {
    return set("client_encoding", val);
}

config::builder& config::builder::connect_timeout(Duration const val) {
    return setInterval("connect_timeout", val);
}

config::builder& config::builder::dbname(std::string const& val) {
    return set("dbname", val);
}

config::builder& config::builder::fallback_application_name(std::string const& val) {
    return set("fallback_application_name", val);
}

config::builder& config::builder::gsslib(std::string const& val) {
    return set("gsslib", val);
}

config::builder& config::builder::host(std::string const& val) {
    return set("host", val);
}

config::builder& config::builder::hostaddr(std::string const& val) {
    return set("hostaddr", val);
}

config::builder& config::builder::keepalives(bool const val) {
    return enable("keepalives", val);
}

config::builder& config::builder::keepalives_count(int const val) {
    return setNumber("keepalives_count", val);
}

config::builder& config::builder::keepalives_idle(Duration const val) {
    return setInterval("keepalives_idle", val);
}

config::builder& config::builder::keepalives_interval(Duration const val) {
    return setInterval("keepalives_interval", val);
}

config::builder& config::builder::krbsrvname(std::string const& val) {
    return set("krbsrvname", val);
}

config::builder& config::builder::options(std::string const& val) {
    return set("options", val);
}

config::builder& config::builder::passfile(std::string const& val) {
    return set("passfile", val);
}

config::builder& config::builder::password(std::string const& val) {
    return set("password", val);
}

config::builder& config::builder::port(int const val) {
    return setNumber("port", val);
}

config::builder& config::builder::requirepeer(std::string const& val) {
    return set("requirepeer", val);
}

config::builder& config::builder::requiressl(bool const val) {
    return enable("requiressl", val);
}

config::builder& config::builder::service(std::string const& val) {
    return set("service", val);
}

config::builder& config::builder::sslcert(std::string const& val) {
    return set("sslcert", val);
}

config::builder& config::builder::sslcompression(bool const val) {
    return enable("sslcompression", val);
}

config::builder& config::builder::sslcrl(std::string const& val) {
    return set("sslcrl", val);
}

config::builder& config::builder::sslkey(std::string const& val) {
    return set("sslkey", val);
}

config::builder& config::builder::sslmode(SslMode const val) {
    return set("sslmode", [val] {
        switch (val) {
            case SslMode::ALLOW: {
                return "allow";
            }
            case SslMode::DISABLE: {
                return "disable";
            }
            case SslMode::PREFER: {
                return "prefer";
            }
            case SslMode::REQUIRE: {
                return "require";
            }
            case SslMode::VERIFY_CA: {
                return "verify-ca";
            }
            case SslMode::VERIFY_FULL: {
                return "verify-full";
            }
            case SslMode::DEFAULT: {
                break;
            }
        }
        return "prefer";
    }());
}

config::builder& config::builder::sslrootcert(std::string const& val) {
    return set("sslrootcert", val);
}

config::builder& config::builder::target_session_attrs(TargetSession const val) {
    return set("target_session_attrs", [val] {
        switch (val) {
            case TargetSession::ANY: {
                return "any";
            }
            case TargetSession::READ_WRITE: {
                return "read-write";
            }
            case TargetSession::DEFAULT: {
                break;
            }
        }
        return "any";
    }());
}

config::builder& config::builder::user(std::string const& val) {
    return set("user", val);
}

config::builder& config::builder::enable(std::string const& key, bool const val) {
    return set(key, val ? "1" : "0");
}

config::builder& config::builder::setNumber(std::string const& key, int const val) {
    return set(key, std::to_string(val));
}

config::builder& config::builder::setInterval(std::string const& key, Duration const val) {
    using std::chrono::duration_cast;
    using std::chrono::seconds;
    return set(key, std::to_string(duration_cast<seconds>(val).count()));
}

config::builder& config::builder::set(std::string const& key, std::string const& val) {
    cfg_.params_[key] = val;
    return *this;
}

config config::builder::build() {
    for (auto const& param : cfg_.params_) {
        cfg_.keys_.push_back(param.first.c_str());
        cfg_.vals_.push_back(param.second.c_str());
    }
    cfg_.keys_.push_back(nullptr);
    cfg_.vals_.push_back(nullptr);
    return std::move(cfg_);
}

}  // namespace pg
