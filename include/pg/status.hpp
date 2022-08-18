#pragma once

#include <memory>
#include <libpq-fe.h>

namespace pg {

class consumer;

class status
{
    public:
    
        status(status const& other) = delete;
        status& operator=(status const& other) = delete;
        status(status&& other) noexcept;
        status& operator=(status&& other) noexcept;
        ~status() noexcept;

        bool ok() const;
        bool done() const;
        bool empty() const;

        int size() const;
        int effect() const;
        const char* message() const;
        const char* describe() const;
        ExecStatusType type() const;

        PGresult* native() const;

    protected:
    
        friend class connection;
        friend class consumer;

        explicit status(PGresult* handle);
        explicit status(PGresult* handle, consumer*);

        void check() const;

    private:
        std::unique_ptr<PGresult, void (*)(PGresult*)> handle_;
};

}  // namespace pg
