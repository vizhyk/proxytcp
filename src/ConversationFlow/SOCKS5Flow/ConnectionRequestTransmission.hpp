#ifndef PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#define PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
#include "../ConversationFlow.hpp"
#include <netdb.h>
#include "fcntl.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>


namespace Proxy::SOCKS5Flow
{
    class ConnectionRequestTransmission : public ConversationFlow
    {
    public:
        ~ConnectionRequestTransmission() override = default;
        static Status TryConnectToTheServer(int32_t serverSocket, const uint8_t *serverAddress, uint16_t serverPort, uint8_t addressType, uint8_t serverAddressSize) noexcept;
        static Status CreateSocketForForwardingByIP(int32_t& socketForForwarding, int32_t destinationPort, const uint8_t* addr) noexcept;
        static Status CreateSocketForForwardingByHostname(int32_t& socketForForwarding, int32_t destinationPort, const uint8_t* hostname) noexcept;
        static Status MakeSocketNonblocking(int32_t socket) noexcept;
        static int32_t GenerateConnectionRequestReply(const uint8_t *buffer, size_t buffersize, ByteStream &GeneratedReply) noexcept;

        std::unique_ptr<ConversationFlow> PerformTransaction(Connection &clientConnection, Connection &serverConnection) noexcept override;
    };
}



#endif //PROXYTCP_CONNECTIONREQUESTTRANSMISSION_HPP
