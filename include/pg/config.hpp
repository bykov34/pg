#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace pg {

enum class SslMode {
    ALLOW,
    DISABLE,
    PREFER,
    REQUIRE,
    VERIFY_CA,
    VERIFY_FULL,
    DEFAULT,
};

enum class TargetSession {
    ANY,
    READ_WRITE,
    DEFAULT,
};

class config {
public:
    class builder;

    config(config const& other) = delete;
    config& operator=(config const& other) = delete;
    config(config&& other) noexcept;
    config& operator=(config&& other) noexcept;
    ~config() noexcept;

    static config build();

    // libpq interface adapters.
    char const* const* keys() const;
    char const* const* values() const;

private:
    explicit config();

    std::vector<char const*>           keys_;
    std::vector<char const*>           vals_;
    std::map<std::string, std::string> params_;
};

class config::builder {
public:
    using Duration = std::chrono::high_resolution_clock::duration;

    explicit builder();
    builder(builder const& other) = delete;
    builder& operator=(builder const& other) = delete;
    builder(builder&& other) noexcept;
    builder& operator=(builder&& other) noexcept;
    ~builder() noexcept;

    builder& application_name(std::string const& val);
    builder& client_encoding(std::string const& val);
    builder& connect_timeout(Duration val);
    builder& dbname(std::string const& val);
    builder& fallback_application_name(std::string const& val);
    builder& gsslib(std::string const& val);
    builder& host(std::string const& val);
    builder& hostaddr(std::string const& val);
    builder& keepalives(bool val);
    builder& keepalives_count(int val);
    builder& keepalives_idle(Duration val);
    builder& keepalives_interval(Duration val);
    builder& krbsrvname(std::string const& val);
    builder& options(std::string const& val);
    builder& passfile(std::string const& val);
    builder& password(std::string const& val);
    builder& port(int val);
    builder& requirepeer(std::string const& val);
    builder& requiressl(bool val);
    builder& service(std::string const& val);
    builder& sslcert(std::string const& val);
    builder& sslcompression(bool val);
    builder& sslcrl(std::string const& val);
    builder& sslkey(std::string const& val);
    builder& sslmode(SslMode val);
    builder& sslrootcert(std::string const& val);
    builder& target_session_attrs(TargetSession val);
    builder& user(std::string const& val);

    builder& enable(std::string const& key, bool val);
    builder& setNumber(std::string const& key, int val);
    builder& setInterval(std::string const& key, Duration val);
    builder& set(std::string const& key, std::string const& val);

    config build();

private:
    config cfg_;
};

}  // namespace pg
