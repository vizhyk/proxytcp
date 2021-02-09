#ifndef PROXYTCP_CONVERSATIONFLOW_HPP
#define PROXYTCP_CONVERSATIONFLOW_HPP

#include "ByteStream/ByteStream.hpp"
#include <memory>

#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"

namespace Proxy
{
    class ConversationFlow
    {
    public:
        virtual ~ConversationFlow() noexcept = default;
        virtual std::unique_ptr<ConversationFlow> PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept = 0;

    protected:
        static Status ReadAllDataFromConnection(Connection& connection) noexcept;

        static Status SendAllDataToConnection(const ByteStream& data, Connection& recipientConnection) noexcept;
    };

}

#endif //PROXYTCP_CONVERSATIONFLOW_HPP
