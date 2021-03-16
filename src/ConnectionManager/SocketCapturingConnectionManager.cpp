#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "ConversationManager/SocketCapturingConversationManager.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"
#include "SocketCapturingConnectionManager.hpp"

#include <utility>

namespace Proxy
{
    SocketCapturingConnectionManager::SocketCapturingConnectionManager(std::string outputFilePath) noexcept
        : m_outputFilePath(std::move(outputFilePath))
    {
        m_conversationManager = std::make_unique<SocketCapturingConversationManager>();
    }

    Status SocketCapturingConnectionManager::FindPipelineAndPerformTransaction(int32_t sockfd)
    {
        Status status;

        const auto pipeline =  dynamic_cast<CapturingConversationPipeline*>(FindConversationPipeline(sockfd).get());
        if(pipeline == nullptr)
        {
            status = Status(Status::Error::NoPipelineFound);
            return status;
        }

        if(!pipeline->PCAPFile().IsOpened())
        {
            pipeline->PCAPFile().Open(m_outputFilePath);
            pipeline->PCAPFile().Write(PCAP::PCAPGenerator::GeneratePCAPGlobalHeader());
            pipeline->PCAPFile().Write(PCAP::PCAPGenerator::Generate3WayTCPHandshake());
        }

        pipeline->PerformTransaction(sockfd);

        return status;
    }



}
