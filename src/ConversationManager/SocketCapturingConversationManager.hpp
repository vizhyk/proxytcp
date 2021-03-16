#ifndef PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP

#include "SocketConversationManager.hpp"

namespace Proxy
{
    class SocketCapturingConversationManager : public SocketConversationManager
    {
    public:
        std::shared_ptr<ConversationPipeline> AddNewPipeline(int32_t clientSockfd, int32_t epollfd) override;
    };
}




#endif //PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP
