#ifndef PROXYTCP_SOCKETCONNECTION_HPP
#define PROXYTCP_SOCKETCONNECTION_HPP
#include "Connection/Connection.hpp"

namespace Proxy
{
    class SocketConnection: public Connection
    {
    public:
        SocketConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;
        Status ReadData() override;
        Status SendData(const ByteStream& data) override;
        virtual Status SendDataTo(const ByteStream& data, SocketConnection& recipientConnection) noexcept;

    };
}

#endif //PROXYTCP_SOCKETCONNECTION_HPP
