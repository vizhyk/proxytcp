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

    void CapturingConversationPipeline::InitPCAPClientData(const PCAPData& pcapData) noexcept
    {
        m_clientPCAPData = pcapData;
    }

    void CapturingConversationPipeline::InitPCAPServerData(const PCAPData& pcapData) noexcept
    {
        m_serverPCAPData = pcapData;
    }

}