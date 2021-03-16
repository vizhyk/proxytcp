#ifndef PROXYTCP_CONVERSATIONPIPELINE_HPP
#define PROXYTCP_CONVERSATIONPIPELINE_HPP

#include "ConversationFlow/ConversationFlow.hpp"
#include "ConversationFlow/SOCKS5Flow/ClientHelloTransmission.hpp"
#include "ConversationPipeline/PayloadBuffer/PayloadBuffer.hpp"
#include "Connection/SocketConnection.hpp"

namespace Proxy
{

    class SocketConversationManager;

    class ConversationPipeline : public std::enable_shared_from_this<ConversationPipeline>
    {
    public:
        virtual ~ConversationPipeline() = default;

        ConversationPipeline(int32_t epollfd, SocketConversationManager& conversationManager) noexcept;
        ConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, SocketConversationManager& conversationManager) noexcept;

        ConversationPipeline(ConversationPipeline&& rhs) noexcept = delete;
        ConversationPipeline& operator=(ConversationPipeline&& rhs) noexcept = delete;

    public:
        template<typename PayloadType>
        PayloadType& InitPayloadAs()
        {
            m_payload = std::make_unique<PayloadType>();
            return *static_cast<PayloadType*>(m_payload.get());
        }

        template<typename PayloadType>
        PayloadType& PayloadAs()
        {
            return *static_cast<PayloadType*>(m_payload.get());
        }

        virtual void PerformTransaction(int32_t sockfdWithEvent) noexcept;
        virtual void InitServerConnection(int32_t sockfd) noexcept;
        virtual void InitClientConnection(int32_t sockfd) noexcept;

        int32_t GetEpollfd()      const noexcept;
        int32_t GetClientSockfd() const noexcept;
        int32_t GetServerSockfd() const noexcept;
        ConversationFlow::FlowState GetConversationFlowState() const noexcept;

        bool IsClientConnectionInitialized() const noexcept;
        bool IsServerConnectionInitialized() const noexcept;

        SocketConversationManager& PipelineManager() noexcept;
        bool PayloadIsInitialized() const noexcept;

    protected:
        std::unique_ptr<SocketConnection> m_clientConnection;
        std::unique_ptr<SocketConnection> m_serverConnection;
        std::unique_ptr<ConversationFlow> m_conversationFlow;
        std::unique_ptr<PayloadBuffer> m_payload;
        SocketConversationManager& m_conversationManager;
        int32_t m_epollfd;



    };

}




#endif //PROXYTCP_CONVERSATIONPIPELINE_HPP
