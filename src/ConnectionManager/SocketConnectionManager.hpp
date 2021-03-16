#ifndef PROXYTCP_SOCKETCONNECTIONMANAGER_HPP
#define PROXYTCP_SOCKETCONNECTIONMANAGER_HPP

#include "ConnectionManager.hpp"
#include <sys/epoll.h>

namespace Proxy
{
    class SocketConnectionManager : public ConnectionManager
    {
    public:
        SocketConnectionManager() noexcept;
        ~SocketConnectionManager() override = default;

        static Status EpollAdd(int32_t epollfd, int32_t sockfd, uint32_t eventFlags) noexcept;
        static Status EpollAdd(int32_t epollfd, int32_t sockfd, uint32_t eventFlags, ConversationPipeline* pipeline) noexcept;
        static Status EpollCreate(int32_t& epollfd) noexcept;
        static Status EpollWait(int32_t epollfd, epoll_event* epollEvents, uint32_t epollEventsSize, int32_t& socketsWithEvent) noexcept;
        static Status BindSocketToPort(int32_t& sockfd, uint16_t port) noexcept;
        static Status MakeSocketNonblocking(int32_t sockfd) noexcept;

        std::shared_ptr<ConversationPipeline> FindConversationPipeline(int32_t sockfd) noexcept;
        std::shared_ptr<ConversationPipeline> AddConversationPipeline(int32_t clientSockfd, int32_t epollfd);

        Status AcceptNewConnectionSocket(int32_t listeningSocket, int32_t epollfd) noexcept;
        Status ProcessConnections(uint16_t port) override;
        Status FindPipelineAndPerformTransaction(int32_t sockfd) override;

    };
}



#endif //PROXYTCP_SOCKETCONNECTIONMANAGER_HPP
