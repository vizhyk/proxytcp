#ifndef PROXYTCP_CONNECTIONMANAGER_HPP
#define PROXYTCP_CONNECTIONMANAGER_HPP

#include "src/ConversationManager/ConversationManager.hpp"
#include <memory>

namespace Proxy
{
    class ConnectionManager : public NonCopyable
    {
    public:
        virtual ~ConnectionManager() = default;
        virtual Status ProcessConnections(uint16_t port) = 0;

    protected:
        std::unique_ptr<ConversationManager> m_conversationManager;
    };
}



#endif //PROXYTCP_CONNECTIONMANAGER_HPP
