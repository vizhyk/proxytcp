#ifndef PROXYTCP_CONNECTION_HPP
#define PROXYTCP_CONNECTION_HPP

#include <memory>
#include <csignal>
#include <sys/socket.h>
#include "ByteStream/ByteStream.hpp"


namespace Proxy
{
    class ConversationPipeline;

    class Connection
    {
    public:

        enum class ConnectionState : uint8_t
        {
            Connected = 0x00,
            NotConnected = 0xff,
        };

    public:
        Connection(int32_t socket, ConnectionState state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;
        virtual ~Connection();

        virtual Status ReadData() = 0;
        virtual Status SendData(const ByteStream& data) = 0;

        Connection(Connection&& connection) noexcept = delete;
        Connection& operator=(Connection&& rhs) noexcept = delete;

    public:
        ConnectionState GetState() const noexcept;
        int32_t GetSocketfd() const noexcept;

        void ChangeState(ConnectionState state) noexcept;

        ByteStream& Buffer() noexcept;
        std::weak_ptr<ConversationPipeline>& Pipeline() noexcept;
    protected:
        int32_t m_socket;
        ConnectionState m_state;
        ByteStream m_buffer;
        std::weak_ptr<ConversationPipeline> m_pipeline;

    };

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept;
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept;

}

#endif //PROXYTCP_CONNECTION_HPP
