#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "ConversationManager/SocketCapturingConversationManager.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"
#include "SocketCapturingConnectionManager.hpp"

#include <utility>

namespace Proxy
{
    SocketCapturingConnectionManager::SocketCapturingConnectionManager(const std::string& outputFilePath) noexcept
    {
        m_conversationManager = std::make_unique<SocketCapturingConversationManager>();

        dynamic_cast<SocketCapturingConversationManager*>(m_conversationManager.get())->OpenPCAPFile(outputFilePath);
        dynamic_cast<SocketCapturingConversationManager*>(m_conversationManager.get())->PCAPCapturingFile().Write(PCAP::PCAPGenerator::GeneratePCAPGlobalHeader());
    }

//    Status SocketCapturingConnectionManager::FindPipelineAndPerformTransaction(int32_t sockfd)
//    {
//        Status status;
//
//        const auto pipeline =  dynamic_cast<CapturingConversationPipeline*>(FindConversationPipeline(sockfd).get());
//        if(pipeline == nullptr)
//        {
//            status = Status(Status::Error::NoPipelineFound);
//            return status;
//        }
//
//        status = pipeline->PerformTransaction(sockfd);
//        if(status == Status::Success::DataTransferEnded)
//        {
//            m_conversationManager->ErasePipeline(sockfd);
//        }
//
//        return status;
//    }



}
