#ifndef PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP
#define PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP

#include "Connection/SocketConnection.hpp"
#include "ConversationFlow/ConversationFlow.hpp"

namespace Proxy::TLSFlow
{
    class TLSReceivingServerHello : public ConversationFlow
    {
    public:
        std::unique_ptr<ConversationFlow>
        PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;
    };
}

#endif //PROXYTCP_TLSRECEIVINGSERVERHELLO_HPP
