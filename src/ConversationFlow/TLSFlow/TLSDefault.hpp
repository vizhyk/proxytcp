#ifndef PROXYTCP_TLSDEFAULT_HPP
#define PROXYTCP_TLSDEFAULT_HPP


#include "ConversationFlow/ConversationFlow.hpp"

namespace Proxy::TLSFlow
{
    class TLSDefault : public ConversationFlow
    {
    public:
        ~TLSDefault() override = default;

    public:
        std::unique_ptr<ConversationFlow>
        PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept override;
    };
}



#endif //PROXYTCP_TLSDEFAULT_HPP
