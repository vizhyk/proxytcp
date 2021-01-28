#include <netinet/in.h>
#include <fcntl.h>
#include <iostream>
#include <thread>
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"

#include "ConnectionManager.hpp"

namespace Proxy
{
    Status
    ConnectionManager::EpollAdd(int32_t epollfd, int32_t sockfd, uint32_t eventFlags) noexcept
    {
        Status status;

        epoll_event ev {};

        ev.events = eventFlags;
        ev.data.fd = sockfd;

        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
        {
            status = Status::Error::BadEpollCTL;
            return status;
        }

        return status;
    }

    Status
    ConnectionManager::EpollAdd(int32_t epollfd, int32_t sockfd, uint32_t eventFlags, ConversationPipeline* pipeline) noexcept
    {
        Status status;

        epoll_event ev {};

        ev.events = eventFlags;
        ev.data.fd = sockfd;

//        ev.data.ptr = pipeline;
//        if(ev.data.ptr == nullptr)
//        {
//            status = Status(Status::Error::NoConversationPipelineFound);
//            return status;
//        }

        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev) == -1)
        {
            status = Status::Error::BadEpollCTL;
            return status;
        }

        return status;
    }

    Status
    ConnectionManager::BindSocketToPort(int32_t& _sockfd, uint16_t port) noexcept
    {
        Status status {};

        sockaddr_in socketData {};

        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(_sockfd == -1)
        {
            status = Status(Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        int32_t enable {1};
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
            status = Status(Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        status = MakeSocketNonblocking(_sockfd);
        if(status.Failed()) { return status; }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));

        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(port);

        if(bind(_sockfd, reinterpret_cast<sockaddr*>(&socketData), sizeof(socketData)) == -1)
        {
            status = Status(Status::Error::BadBindListeningPortToSocket);
            return status;
        }

        if(listen(_sockfd, 5) == -1)
        {
            status = Status(Status::Error::MarkSocketPassive);
            return status;
        }

        return status;
    }

    Status
    ConnectionManager::MakeSocketNonblocking(int32_t sockfd) noexcept
    {
        Status status {};
        int32_t flags = fcntl(sockfd, F_GETFL, 0);
        auto result = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
        if(result < 0)
        {
            status = Status::Error::BadMakingSocketNonblocking;
            return status;
        }
        return status;
    }

    void
    ConnectionManager::PrintStatus(const Status& status) noexcept
    {
        std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
    }

    void
    ConnectionManager::PrintStatusAndTerminateProcess(const Status& status) noexcept
    {
        std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
        exit(status.Code());
    }

    std::shared_ptr<ConversationPipeline>
    ConnectionManager::FindConversationPipeline(int32_t sockfd) noexcept
    {
        return m_conversationManager.FindPipelineBySockfd(sockfd);
    }

    Status
    ConnectionManager::ProcessConnections()
    {
        Status status;

        int32_t epollfd = 0;
        int32_t listeningSocket = 0;
        int32_t socketsWithEvent = 0;
        const uint32_t EVENTS_SIZE = 100;

        epoll_event events[EVENTS_SIZE];

        status = BindSocketToPort(listeningSocket,1080);
        if(status.Failed()) { return status; }

        status = EpollCreate(epollfd);
        if(status.Failed()) { return status; }

        status = EpollAdd(epollfd,listeningSocket, EPOLLIN);
        if(status.Failed()) { return status; }

        while(true)
        {
            status = EpollWait(epollfd, events,EVENTS_SIZE, socketsWithEvent);
            if(status.Failed()) { return status; }

            for(int32_t sockfdID = 0; sockfdID < socketsWithEvent; ++sockfdID)
            {
                if(events[sockfdID].data.fd == listeningSocket)
                {
                    status = AcceptNewConnectionSocket(listeningSocket, epollfd);
                    if(status.Failed()) { return status; }

                    continue;
                }

                const auto pipeline = FindConversationPipeline(events[sockfdID].data.fd);
                if(pipeline == nullptr)
                {
                    status = Status(Status::Error::NoPipelineFound);
                    return status;
                }

                pipeline->PerformTransaction(events[sockfdID].data.fd);
            }

        }

        return status;
    }

    Status
    ConnectionManager::EpollCreate(int32_t& epollfd) noexcept
    {
        Status status;

        epollfd = epoll_create(1);
        if(epollfd == -1)
        {
            status = Status(Status::Error::BadEpollCreateInstance);
            return status;
        }

        return status;
    }

    Status
    ConnectionManager::EpollWait(int32_t epollfd, epoll_event* epollEvents, uint32_t epollEventsSize, int32_t& socketsWithEvent) noexcept
    {
        Status status;

        socketsWithEvent = epoll_wait(epollfd, epollEvents, epollEventsSize, -1);
        if(socketsWithEvent == -1)
        {
            status = Status(Status::Error::BadEpollWait);
            return status;
        }

        return status;
    }

    Status
    ConnectionManager::AcceptNewConnectionSocket(int32_t listeningSocket, int32_t epollfd) noexcept
    {
        Status status;

        auto newConnectionSocket = accept(listeningSocket, nullptr, nullptr);
        if(newConnectionSocket == -1)
        {
            status = Status::Error::BadConnectionFromListeningSocket;
            return status;
        }

        status = MakeSocketNonblocking(newConnectionSocket);
        if(status.Failed()) { return status; }

        auto pipelinePtr = AddConversationPipeline(newConnectionSocket, epollfd);
        if(pipelinePtr == nullptr)
        {
            status = Status(Status::Error::NoConversationPipelineCreated);
            return status;
        }

        status = EpollAdd(epollfd,newConnectionSocket, EPOLLIN | EPOLLET, pipelinePtr.get() );
        if(status.Failed()) { return status; }

        return status;
    }

    std::shared_ptr<ConversationPipeline>
    ConnectionManager::AddConversationPipeline(int32_t clientSockfd, int32_t epollfd)
    {
        return m_conversationManager.AddNewPipeline(clientSockfd,epollfd);
    }

    std::shared_ptr<ConversationPipeline> ConnectionManager::LinkSockfdToPipeline(int32_t sockfd, std::shared_ptr<ConversationPipeline> pipelinePtr)
    {
        return m_conversationManager.LinkSockfdToExistingPipeline(sockfd, pipelinePtr);
    }

}