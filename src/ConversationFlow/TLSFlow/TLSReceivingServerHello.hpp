#ifndef PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP
#define PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP

#include "ConversationFlow/ConversationFlow.hpp"

namespace Proxy::TLSFlow
{
    class TLSReceivingServerHello : public ConversationFlow
    {
    public:
        ~TLSReceivingServerHello() noexcept override = default;

    public:
        std::unique_ptr<ConversationFlow>
        PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;
    };
}

#endif //PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP
