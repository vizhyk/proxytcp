#ifndef PROXYTCP_SOCKETCONVERSATIONMANAGER_HPP
#define PROXYTCP_SOCKETCONVERSATIONMANAGER_HPP

#include "src/ConversationPipeline/ConversationPipeline.hpp"
#include "src/Utilities/NonCopyable.hpp"
#include "Connection/SocketConnection.hpp"


#include <map>
#include <memory>

namespace Proxy
{
    class ConversationManager : public NonCopyable
    {
    public:
        virtual  ~ConversationManager() = default;
        virtual std::shared_ptr<ConversationPipeline> LinkSockfdToExistingPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline>& pipelinePtr) = 0;
        virtual std::shared_ptr<ConversationPipeline> FindPipelineBySockfd(int32_t sockfd) noexcept  = 0;
    };

    class SocketConversationManager : public ConversationManager
    {
    public:
        virtual std::shared_ptr<ConversationPipeline> AddNewPipeline(int32_t clientSockfd, int32_t epollfd);
        std::shared_ptr<ConversationPipeline> LinkSockfdToExistingPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline>& pipelinePtr)override;
        std::shared_ptr<ConversationPipeline> FindPipelineBySockfd(int32_t sockfd) noexcept override;

    protected:
        std::map<int32_t, std::shared_ptr<ConversationPipeline>> pipelines;
    };


}



#endif //PROXYTCP_SOCKETCONVERSATIONMANAGER_HPP


