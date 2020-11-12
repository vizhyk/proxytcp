#include "Connection.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket, Connection::Side type) noexcept
        : socket{socket}, type{type} {}

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() == rhs.GetSocketfd(); }
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept { return !(lhs == rhs); }
}


