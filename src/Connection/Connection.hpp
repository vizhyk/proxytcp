#ifndef PROXYTCP_CONNECTION_HPP
#define PROXYTCP_CONNECTION_HPP
#include <cstdint>
#include "ByteStream/ByteStream.hpp"


namespace Proxy
{
    class Connection
    {
    public:

        enum class ConnectionState : uint8_t
        {
            Connected = 0x00,
            NotConnected = 0xff,
        };

    public:
        Connection() noexcept = default;
        Connection(int32_t socket, ConnectionState state) noexcept;
        void ChangeState(ConnectionState state) noexcept;
        virtual ~Connection() = default;

    public:
        void SetRequiredBytes(std::size_t newRequiredBytes) noexcept;

        int32_t GetSocketfd() const noexcept { return m_socket; }
        std::size_t GetRequiredBytes() const noexcept { return m_requiredBytes; }
        ByteStream& Buffer()  noexcept { return m_buffer; }
        ConnectionState GetState() const noexcept { return m_state; }


    private:
        int32_t m_socket;
        std::size_t m_requiredBytes;
        ConnectionState m_state;
        ByteStream m_buffer;
    };

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept;
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept;

}

#endif //PROXYTCP_CONNECTION_HPP
