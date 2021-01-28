#ifndef PROXYTCP_CONVERSATIONMANAGER_HPP
#define PROXYTCP_CONVERSATIONMANAGER_HPP

#include "src/ConversationPipeline/ConversationPipeline.hpp"
#include "src/Utilities/NonCopyable.hpp"

#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"

#include <map>
#include <memory>

namespace Proxy
{
    class ConversationManager : public NonCopyable
    {
    public:
        std::shared_ptr<ConversationPipeline> AddNewPipeline(int32_t clientSockfd, int32_t epollfd);
        std::shared_ptr<ConversationPipeline> LinkSockfdToExistingPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline>& pipelinePtr);
        std::shared_ptr<ConversationPipeline> FindPipelineBySockfd(int32_t sockfd) noexcept;

        Status RemoveUnactivePipeline(int32_t sockfd) noexcept;

    private:
        std::map<int32_t , std::shared_ptr<ConversationPipeline>> pipelines;
    };
}



#endif //PROXYTCP_CONVERSATIONMANAGER_HPP
