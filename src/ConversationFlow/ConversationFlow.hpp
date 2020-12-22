#ifndef PROXYTCP_CONVERSATIONFLOW_HPP
#define PROXYTCP_CONVERSATIONFLOW_HPP

#include "Status.hpp"
#include "Connection/Connection.hpp"
#include "ByteStream/ByteStream.hpp"
#include "src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp"
#include "src/Utilities/Constants.hpp"
#include "src/Utilities/SOCKS5.hpp"
#include <memory>

namespace Proxy
{
    class ConversationFlow
    {
    public:
        virtual ~ConversationFlow() noexcept = default;
        virtual std::unique_ptr<ConversationFlow> PerformTransaction(Connection& clientConnection, Connection& serverConnection) noexcept = 0;

    protected:
        static Status ReadAllDataFromConnection(Connection& connection) noexcept;
        static Status SendAllDataToConnection(const ByteStream& data, Connection& recipientConnection) noexcept;

    };

}



#endif //PROXYTCP_CONVERSATIONFLOW_HPP