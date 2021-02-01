#ifndef PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP
#define PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP

#include "ConversationFlow/ConversationFlow.hpp"

namespace Proxy::TLSFlow
{
    class TLSReceivingClientHello : public ConversationFlow
    {
    public:
        ~TLSReceivingClientHello() noexcept override = default;

    public:
        std::unique_ptr<ConversationFlow>
        PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;
    };
}



#endif //PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP