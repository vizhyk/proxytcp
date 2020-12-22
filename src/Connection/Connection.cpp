#include "Connection.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket, ConnectionState state) noexcept
        : m_socket(socket), m_state(state), m_requiredBytes(0)
    {}

    void Connection::ChangeState(ConnectionState state) noexcept
    {
        m_state = state;
    }

    void Connection::SetRequiredBytes(std::size_t newRequiredBytes) noexcept
    {
        m_requiredBytes = newRequiredBytes;
    }


    bool operator==(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() == rhs.GetSocketfd(); }
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept { return !(lhs == rhs); }

}

