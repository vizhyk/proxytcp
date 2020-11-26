#include "Connection.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket) noexcept
        : m_socket{m_socket}  {}

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() == rhs.GetSocketfd(); }
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept { return !(lhs == rhs); }
}

