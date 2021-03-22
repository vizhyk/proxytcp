#include "ConversationManager/SocketCapturingConversationManager.hpp"
#include "Connection/SocketCapturingConnection.hpp"
#include "CapturingConversationPipeline.hpp"

namespace Proxy
{

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, SocketConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, conversationManager), m_clientPCAPData(), m_serverPCAPData(), m_pcapfile(dynamic_cast<SocketCapturingConversationManager*>(&conversationManager)->PCAPCapturingFile())
    {

    }

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, SocketConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, std::move(flow), conversationManager), m_clientPCAPData(), m_serverPCAPData(), m_pcapfile(dynamic_cast<SocketCapturingConversationManager*>(&conversationManager)->PCAPCapturingFile())
    {
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

    PCAP::PCAPCapturingFile& CapturingConversationPipeline::PCAPFile() noexcept
    {
        return m_pcapfile;
    }

    PCAPData& CapturingConversationPipeline::ClientPCAPData() noexcept
    {
        return m_clientPCAPData;
    }

    PCAPData& CapturingConversationPipeline::ServerPCAPData() noexcept
    {
        return m_serverPCAPData;
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
        m_clientPCAPData = pcapData;
    }

    void CapturingConversationPipeline::InitPCAPServerData(const PCAPData& pcapData) noexcept
    {
        m_serverPCAPData = pcapData;
    }

    void CapturingConversationPipeline::InitPCAPClientData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept
    {
        m_clientPCAPData.m_sequenceNumber = sequenceNumber;
        m_clientPCAPData.m_acknowledgmentNumber = acknowledgmentNumber;
        m_clientPCAPData.m_ipv4 = IPv4;
        m_clientPCAPData.m_timestamp = {0, 0};
    }

    void CapturingConversationPipeline::InitPCAPServerData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept
    {
        m_serverPCAPData.m_sequenceNumber = sequenceNumber;
        m_serverPCAPData.m_acknowledgmentNumber = acknowledgmentNumber;
        m_serverPCAPData.m_ipv4 = IPv4;
        m_serverPCAPData.m_timestamp = {0, 0};

    }
}