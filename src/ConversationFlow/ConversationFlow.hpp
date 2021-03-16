#ifndef PROXYTCP_CONVERSATIONFLOW_HPP
#define PROXYTCP_CONVERSATIONFLOW_HPP

#include "ByteStream/ByteStream.hpp"
#include <memory>
#include <iostream>
#include "Connection/SocketConnection.hpp"

namespace Proxy
{
    class ConversationFlow
    {
    public:
        enum class FlowState : uint8_t
        {
            SOCKS5ClientHello = 0x0a,
            SOCKS5ConnectionRequest = 0xb,
            TLSClientHello = 0x0f,
            TLSServerHello = 0xf0,
            TLSDefault = 0xff,
        };
    public:
        virtual ~ConversationFlow() noexcept = default;
        virtual std::unique_ptr<ConversationFlow> PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept = 0;
        FlowState GetState() noexcept { return m_flowState; }

    protected:
        static Status ReadAllDataFromConnection(SocketConnection& connection) noexcept;
        static Status SendAllDataFromConnection(const ByteStream& data, SocketConnection& connection) noexcept;
        static Status SendAllDataToConnection(const ByteStream& data, SocketConnection& recipient) noexcept;

        static void LogData(const std::string& sender, const ByteStream& data) noexcept;

    protected:
        FlowState m_flowState {};
    };

}

#endif //PROXYTCP_CONVERSATIONFLOW_HPP
