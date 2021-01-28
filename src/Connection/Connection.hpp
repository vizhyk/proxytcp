#ifndef PROXYTCP_CONNECTION_HPP
#define PROXYTCP_CONNECTION_HPP


#include <memory>
#include "ByteStream/ByteStream.hpp"


namespace Proxy
{
    class ConversationPipeline;

    class Connection
    {
        friend class ConnectionTest_ChangeState_Test;
        friend class ConnectionTest_Connection_Test;
        friend class ConnectionTest_ChangeSockfd_Test;
        friend class ConnectionTest_GetSocketfd_Test;
        friend class ConnectionTest_Buffer_Test;
        friend class ConnectionTest_shared_Pipeline_Test;

    public:
        enum class ConnectionState : uint8_t
        {
            Connected = 0x00,
            NotConnected = 0xff,
        };

    public:
        Connection() = default;
        Connection(int32_t socket, ConnectionState state, std::shared_ptr<ConversationPipeline>& pipeline) noexcept;
        virtual ~Connection();

        Connection(Connection&& connection) noexcept = delete;
        Connection& operator=(Connection&& rhs) noexcept = delete;

    public:
        ConnectionState GetState() const noexcept;
        int32_t GetSocketfd() const noexcept;

        void ChangeState(ConnectionState state) noexcept;
        void ChangeSockfd(int32_t sockfd) noexcept;

        ByteStream& Buffer() noexcept;
        std::shared_ptr<ConversationPipeline>& Pipeline() noexcept;
    private:

        int32_t m_socket;
        ConnectionState m_state;
        ByteStream m_buffer;
        std::shared_ptr<ConversationPipeline> m_pipeline;

    };

    bool operator==(const Connection& lhs, const Connection& rhs) noexcept;
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept;

}

#endif //PROXYTCP_CONNECTION_HPP

