#include "ConversationPipeline.hpp"
#include "ConversationManager/SocketConversationManager.hpp"
#include "Connection/SocketConnection.hpp"
#include <memory>
#include <utility>

namespace Proxy
{
    ConversationPipeline::ConversationPipeline(int32_t epollfd, SocketConversationManager& conversationManager) noexcept
        : m_payload(nullptr), m_conversationManager(conversationManager), m_epollfd(epollfd)
    {
        m_conversationFlow = std::make_unique<SOCKS5Flow::ClientHelloTransmission>();
    }

    ConversationPipeline::ConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, SocketConversationManager& conversationManager) noexcept
        : m_conversationFlow(std::move(flow)), m_payload(nullptr),m_conversationManager(conversationManager), m_epollfd(epollfd)
    {
    }

    Status
    ConversationPipeline::PerformTransaction(int32_t sockfdWithEvent) noexcept
    {
        auto newFlow = m_conversationFlow->PerformTransaction(*m_clientConnection, *m_serverConnection, m_epollfd, sockfdWithEvent);
        if(newFlow && newFlow->GetState() == ConversationFlow::FlowState::BreakConnection )
        {
            return Status(Status::Success::DataTransferEnded);
        }
        if (newFlow != nullptr)
        {
            m_conversationFlow = std::move(newFlow);
        }

        return Status(Status::Success::Success);
    }

    int32_t
    ConversationPipeline::GetClientSockfd() const noexcept
    {
        return m_clientConnection->GetSocketfd();
    }

    int32_t
    ConversationPipeline::GetServerSockfd() const noexcept
    {
        return (m_serverConnection) ? m_serverConnection->GetSocketfd() : -1;
    }

    int32_t
    ConversationPipeline::GetEpollfd() const noexcept
    {
        return m_epollfd;
    }

    void
    ConversationPipeline::InitClientConnection(int32_t sockfd) noexcept
    {
        m_clientConnection = std::make_unique<SocketConnection>(sockfd, Connection::ConnectionSide::Client, shared_from_this());
    }

    void
    ConversationPipeline::InitServerConnection(int32_t sockfd) noexcept
    {
        m_serverConnection = std::make_unique<SocketConnection>(sockfd, Connection::ConnectionSide::Server, shared_from_this());
    }

    SocketConversationManager&
    ConversationPipeline::PipelineManager() noexcept
    {
        return m_conversationManager;
    }

    bool
    ConversationPipeline::PayloadIsInitialized() const noexcept
    {
        return m_payload != nullptr;
    }

    bool ConversationPipeline::IsClientConnectionInitialized() const noexcept
    {
        return m_clientConnection != nullptr;
    }

    bool ConversationPipeline::IsServerConnectionInitialized() const noexcept
    {
        return m_serverConnection != nullptr;
    }

    ConversationFlow::FlowState ConversationPipeline::GetConversationFlowState() const noexcept
    {
        return m_conversationFlow->GetState();
    }


}
