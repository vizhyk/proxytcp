#include "ConversationManager.hpp"

#include <memory>
#include <sys/socket.h>

namespace Proxy
{

    std::shared_ptr<ConversationPipeline>
    ConversationManager::AddNewPipeline(int32_t clientSockfd, int32_t epollfd)
    {
        auto result = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(clientSockfd),std::forward_as_tuple(new ConversationPipeline(clientSockfd, epollfd, *this)));
        return (result.second) ? result.first->second : nullptr;
    }

    std::shared_ptr<ConversationPipeline> ConversationManager::LinkSockfdToExistingPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline>& pipelinePtr)
    {
        auto result = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(sockfd),std::forward_as_tuple(pipelinePtr));
        return (result.second) ? result.first->second : nullptr;
    }

    std::shared_ptr<ConversationPipeline>
    ConversationManager::FindPipelineBySockfd(int32_t sockfd) noexcept
    {
        auto result = pipelines.find(sockfd);
        if(result == pipelines.end())
            return nullptr;

        return result->second;
    }

    Status
    ConversationManager::RemoveUnactivePipeline(int32_t sockfd) noexcept
    {
        Status status;
        auto pipeline = FindPipelineBySockfd(sockfd);
        if(pipeline != nullptr)
        {
            if(pipeline->IsClientConnectionInitialized())
            {
                shutdown(pipeline->GetClientSockfd(),SHUT_RDWR);
                pipelines.erase(pipeline->GetClientSockfd());
            }

            if(pipeline->IsServerConnectionInitialized())
            {
                shutdown(pipeline->GetServerSockfd(),SHUT_RDWR);
                pipelines.erase(pipeline->GetServerSockfd());
            }
            return status;
        }

        status = Status(Status::Error::NoConversationPipelineFound);
        return status;

    }


}