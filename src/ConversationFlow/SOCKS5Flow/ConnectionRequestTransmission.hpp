#ifndef PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#define PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#include "../ConversationFlow.hpp"
#include "../TLSFlow/TLSReceivingClientHello.hpp"


#include <memory>
#include <Connection/SocketConnection.hpp>


namespace Proxy::SOCKS5Flow
{
    class ConnectionRequestTransmission : public ConversationFlow
    {
    public:
        ConnectionRequestTransmission() noexcept { m_flowState = FlowState::SOCKS5ConnectionRequest; }
    public:
        static Status TryConnectToTheServer(SocketConnection& clientConnection, const uint8_t* serverAddress, uint16_t serverPort, uint8_t addressType, int32_t epollfd, int32_t& serverSockfd) noexcept;
        static int32_t CreateSocketForForwardingByHostname(Status& status, int32_t destinationPort, const uint8_t* hostname, int32_t epollfd) noexcept;
        static Status MakeSocketNonblocking(int32_t socket) noexcept;
        static Status GenerateConnectionRequestReply(const uint8_t *buffer, size_t buffersize, ByteStream &connectionRequestReply) noexcept;
        static Status IsConnectionSucceed(int32_t sockfd) noexcept;
        std::unique_ptr<ConversationFlow> PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;

    private:
        enum class ConnectionState : uint8_t
        {
            Connected = 0x0,
            WaitingForResponse = 0xf0,
            NotConnected = 0xff,
        };

        ConnectionState m_connetionState = ConnectionState::NotConnected;

    };
}



#endif //PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
