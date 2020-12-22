#ifndef PROXYTCP_CLIENTHELLOTRANSMISSION_HPP
#define PROXYTCP_CLIENTHELLOTRANSMISSION_HPP

#include "../ConversationFlow.hpp"
#include "ConnectionRequestTransmission.hpp"

namespace Proxy::SOCKS5Flow
{
    class ClientHelloTransmission : public ConversationFlow
    {
    public:
        ~ClientHelloTransmission() override = default;

        std::unique_ptr<ConversationFlow> PerformTransaction(Connection &clientConnection, Connection &serverConnection) noexcept override;

    private:
        static ByteStream GenerateClientInitiationResponse() noexcept;
    };
}

#endif //PROXYTCP_CLIENTHELLOTRANSMISSION_HPP
