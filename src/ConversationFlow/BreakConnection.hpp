#ifndef PROXYTCP_BREAKCONNECTION_HPP
#define PROXYTCP_BREAKCONNECTION_HPP

#include "ConversationFlow.hpp"

namespace Proxy
{
    class BreakConnection : public ConversationFlow
    {
    public:
        BreakConnection() noexcept
        {
            m_flowState = FlowState::BreakConnection;
        }
        std::unique_ptr<ConversationFlow> PerformTransaction(SocketConnection &clientConnection, SocketConnection &serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override { return nullptr; };
    };
}

#endif //PROXYTCP_BREAKCONNECTION_HPP
