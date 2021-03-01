#ifndef PROXYTCP_CONVERSATIONFLOW_HPP
#define PROXYTCP_CONVERSATIONFLOW_HPP

#include "ByteStream/ByteStream.hpp"
#include <memory>
#include "Connection/SocketConnection.hpp"

namespace Proxy
{
    class ConversationFlow
    {
    public:
        virtual ~ConversationFlow() noexcept = default;
        virtual std::unique_ptr<ConversationFlow> PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept = 0;

    protected:
        static Status ReadAllDataFromConnection(SocketConnection& connection) noexcept;
        static Status SendAllDataFromConnection(const ByteStream& data, SocketConnection& connection) noexcept;
        static Status SendAllDataToConnection(const ByteStream& data, SocketConnection& destination) noexcept;
    };

}

#endif //PROXYTCP_CONVERSATIONFLOW_HPP
