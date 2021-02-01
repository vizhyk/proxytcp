#ifndef PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#define PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#include "../ConversationFlow.hpp"
#include "../TLSFlow/TLSReceivingClientHello.hpp"


#include <memory>


namespace Proxy::SOCKS5Flow
{
    class ConnectionRequestTransmission : public ConversationFlow
    {
    public:
        ~ConnectionRequestTransmission() override = default;
        static Status TryConnectToTheServer(ClientConnection& clientConnection, const uint8_t* serverAddress, uint16_t serverPort, uint8_t addressType, int32_t epollfd, int32_t& serverSockfd) noexcept;
        static Status CreateSocketForForwardingByIP(int32_t socketForForwarding, int32_t destinationPort, const uint8_t* addr) noexcept;
        static int32_t CreateSocketForForwardingByHostname(Status& status, int32_t destinationPort, const uint8_t* hostname, int32_t epollfd) noexcept;
        static Status MakeSocketNonblocking(int32_t socket) noexcept;
        static Status GenerateConnectionRequestReply(const uint8_t *buffer, size_t buffersize, ByteStream &connectionRequestReply) noexcept;
        static Status IsConnectionSucceed(int32_t sockfd) noexcept;
        std::unique_ptr<ConversationFlow> PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;

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
