#include "SocketCapturingConversationManager.hpp"
#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"
namespace Proxy
{
    std::shared_ptr<ConversationPipeline>
    SocketCapturingConversationManager::AddNewPipeline(int32_t clientSockfd, int32_t epollfd)
    {
        auto emplacedPipeline = pipelines.emplace(std::piecewise_construct, std::forward_as_tuple(clientSockfd), std::forward_as_tuple(new CapturingConversationPipeline(epollfd, *this)));
        if(emplacedPipeline.second)
        {
            emplacedPipeline.first->second->InitClientConnection(clientSockfd);

            auto pipelinePCAPData = PCAP::PCAPGenerator::GenerateNewPipelineEndpoints();

            const auto capturingPipeline = dynamic_cast<CapturingConversationPipeline*>(emplacedPipeline.first->second.get());

            capturingPipeline->InitPCAPClientData({1, 1, pipelinePCAPData.client.ipv4, pipelinePCAPData.client.port});
            capturingPipeline->InitPCAPServerData({1, 1, pipelinePCAPData.server.ipv4, pipelinePCAPData.server.port});

            if(m_pcapfile.IsOpened())
            {
                capturingPipeline->PCAPFile().Write(PCAP::PCAPGenerator::Generate3WayTCPHandshake(capturingPipeline->ClientPCAPData(),capturingPipeline->ServerPCAPData()));
            }
            else
            {
                std::cout << "No pcap file opened\n";
                exit(1);
            }

            return emplacedPipeline.first->second;
        }
        return nullptr;
    }

    PCAP::PCAPCapturingFile& SocketCapturingConversationManager::PCAPCapturingFile() noexcept
    {
        return m_pcapfile;
    }

    void SocketCapturingConversationManager::OpenPCAPFile(const std::string& filePath) noexcept
    {
        //TODO: check if I've opened non-empty file!!
        m_pcapfile.Open(filePath, std::ios_base::binary);
        m_pcapfile.Write(nullptr,0);
        m_pcapfile.Close();

        m_pcapfile.Open(filePath, std::ios_base::binary | std::ios_base::app);
        if(!m_pcapfile.IsOpened())
        {
            std::cout << "No file was opened\n";
            exit(1);
        }
    }
}
