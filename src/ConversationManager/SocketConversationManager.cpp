#include "SocketConversationManager.hpp"

#include <memory>
#include <sys/socket.h>

namespace Proxy
{

    std::shared_ptr<ConversationPipeline>
    SocketConversationManager::AddNewPipeline(int32_t clientSockfd, int32_t epollfd)
    {
        auto result = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(clientSockfd),std::forward_as_tuple( new ConversationPipeline(epollfd, *this)));
        if(result.second)
        {
            result.first->second->InitClientConnection(clientSockfd);
            return result.first->second;
        }
        return nullptr;
    }

    std::shared_ptr<ConversationPipeline> SocketConversationManager::LinkSockfdToExistingPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline>& pipelinePtr)
    {
        auto result = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(sockfd),std::forward_as_tuple(pipelinePtr));
        return (result.second) ? result.first->second : nullptr;
    }

    std::shared_ptr<ConversationPipeline>
    SocketConversationManager::FindPipelineBySockfd(int32_t sockfd) noexcept
    {
        auto result = pipelines.find(sockfd);
        if(result == pipelines.end())
            return nullptr;

        return result->second;
    }

    void SocketConversationManager::ErasePipeline(int32_t sockfd) noexcept
    {
        pipelines.erase(sockfd);
    }

}