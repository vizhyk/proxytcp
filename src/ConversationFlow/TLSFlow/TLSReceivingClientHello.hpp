#ifndef PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP
#define PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP

#include "Connection/SocketConnection.hpp"
#include "ConversationFlow/ConversationFlow.hpp"

namespace Proxy::TLSFlow
{
    class TLSReceivingClientHello : public ConversationFlow
    {
    public:
        TLSReceivingClientHello() noexcept { m_flowState = FlowState::TLSClientHello; }
    public:
        std::unique_ptr<ConversationFlow>
        PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;
    };
}



#endif //PROXYTCP_TLSRECEIVINGCLIENTHELLO_HPP
