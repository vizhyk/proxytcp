#include "Connection/SocketCapturingConnection.hpp"
#include "CapturingConversationPipeline.hpp"

namespace Proxy
{

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, ConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, conversationManager)
    {
        m_pcapfile = std::make_unique<PCAP::PCAPCapturingFile>();
    }

    CapturingConversationPipeline::CapturingConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, ConversationManager& conversationManager) noexcept
            : ConversationPipeline(epollfd, std::move(flow), conversationManager)
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

    SYNACKData& CapturingConversationPipeline::ClientSYNACK() noexcept
    {
        return m_clientSYNACKData;
    }

    SYNACKData& CapturingConversationPipeline::ServerSYNACK() noexcept
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
}