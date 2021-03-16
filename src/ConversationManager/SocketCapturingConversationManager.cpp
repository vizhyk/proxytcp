#include "SocketCapturingConversationManager.hpp"
#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"
namespace Proxy
{
    std::shared_ptr<ConversationPipeline>
    SocketCapturingConversationManager::AddNewPipeline(int32_t clientSockfd, int32_t epollfd)
    {
        auto result = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(clientSockfd),std::forward_as_tuple( new CapturingConversationPipeline(epollfd, *this)));
        if(result.second)
        {
            result.first->second->InitClientConnection(clientSockfd);

            auto pipelinePCAPData = PCAP::PCAPGenerator::GenerateNewPipelineEndpoints();

            dynamic_cast<CapturingConversationPipeline*>(result.first->second.get())->InitPCAPClientData({1,1,pipelinePCAPData.client.ipv4,pipelinePCAPData.client.port});
            dynamic_cast<CapturingConversationPipeline*>(result.first->second.get())->InitPCAPServerData({1,1,pipelinePCAPData.server.ipv4,pipelinePCAPData.server.port});
            return result.first->second;
        }
        return nullptr;
    }
}
