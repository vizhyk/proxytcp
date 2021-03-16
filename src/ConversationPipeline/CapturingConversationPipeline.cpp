#include "Connection/SocketCapturingConnection.hpp"
#include "CapturingConversationPipeline.hpp"

namespace Proxy
{

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, SocketConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, conversationManager), m_clientSYNACKData(), m_serverSYNACKData()
    {
        m_pcapfile = std::make_unique<PCAP::PCAPCapturingFile>();
    }

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, SocketConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, std::move(flow), conversationManager), m_clientSYNACKData(), m_serverSYNACKData()
    {
        m_pcapfile = std::make_unique<PCAP::PCAPCapturingFile>();
    }

    void
    CapturingConversationPipeline::InitClientConnection(int32_t sockfd) noexcept
    {
        m_clientConnection = std::make_unique<SocketCapturingConnection>(sockfd, Connection::ConnectionSide::Client, shared_from_this());
    }

    void
    CapturingConversationPipeline::InitServerConnection(int32_t sockfd) noexcept
    {
        m_serverConnection = std::make_unique<SocketCapturingConnection>(sockfd, Connection::ConnectionSide::Server, shared_from_this());
    }

    void CapturingConversationPipeline::OpenPCAPFile(const std::string& filename) noexcept
    {
        m_pcapfile->Open(filename);
    }

    PCAP::PCAPCapturingFile& CapturingConversationPipeline::PCAPFile() noexcept
    {
        return *m_pcapfile;
    }

    PCAPData& CapturingConversationPipeline::ClientPCAPData() noexcept
    {
        return m_clientSYNACKData;
    }

    PCAPData& CapturingConversationPipeline::ServerPCAPData() noexcept
    {
        return m_serverSYNACKData;
    }

    void CapturingConversationPipeline::PerformTransaction(int32_t sockfdWithEvent) noexcept
    {
        auto newFlow = m_conversationFlow->PerformTransaction(*m_clientConnection, *m_serverConnection, m_epollfd, sockfdWithEvent);
        if (newFlow != nullptr)
        {
            m_conversationFlow = std::move(newFlow);
        }
    }

    void CapturingConversationPipeline::InitPCAPClientData(const PCAPData& pcapData) noexcept
    {
        m_clientSYNACKData = pcapData;
    }

    void CapturingConversationPipeline::InitPCAPServerData(const PCAPData& pcapData) noexcept
    {
        m_serverSYNACKData = pcapData;
    }

    void CapturingConversationPipeline::InitPCAPClientData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept
    {
        m_clientSYNACKData.m_sequenceNumber = sequenceNumber;
        m_clientSYNACKData.m_acknowledgmentNumber = acknowledgmentNumber;
        m_clientSYNACKData.m_IPv4 = IPv4;
    }

    void CapturingConversationPipeline::InitPCAPServerData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept
    {
        m_serverSYNACKData.m_sequenceNumber = sequenceNumber;
        m_serverSYNACKData.m_acknowledgmentNumber = acknowledgmentNumber;
        m_serverSYNACKData.m_IPv4 = IPv4;
    }
}