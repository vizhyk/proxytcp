#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "ConversationManager/CapturingConversationManager.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"
#include "SocketCapturingConnectionManager.hpp"

namespace Proxy
{
    SocketCapturingConnectionManager::SocketCapturingConnectionManager() noexcept
    {
        m_conversationManager = std::make_unique<CapturingConversationManager>();
    }

    Status
    SocketCapturingConnectionManager::ProcessConnections(uint16_t port)
    {
        Status status;

        int32_t epollfd = 0;
        int32_t listeningSocket = 0;
        int32_t socketsWithEvent = 0;
        const uint32_t EVENTS_SIZE = 100;

        epoll_event events[EVENTS_SIZE];

        status = BindSocketToPort(listeningSocket,port);
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

                const auto pipeline =  dynamic_cast<CapturingConversationPipeline*>(FindConversationPipeline(events[sockfdID].data.fd).get());
                if(pipeline == nullptr)
                {
                    status = Status(Status::Error::NoPipelineFound);
                    return status;
                }

                if(!pipeline->PCAPFile().IsOpened())
                {
                    pipeline->PCAPFile().Open(m_outputFilePath);
                    pipeline->PCAPFile().Write(PCAP::PCAPGenerator::GeneratePCAPGlobalHeader());
                    pipeline->PCAPFile().Write(PCAP::PCAPGenerator::Generate3WayTCPHandshake());
                }

                pipeline->PerformTransaction(events[sockfdID].data.fd);
            }

        }

        return status;
    }

    void SocketCapturingConnectionManager::SetOutputFilePath(std::string outputFilePath) noexcept
    {
        m_outputFilePath = std::move(outputFilePath);
    }

}
