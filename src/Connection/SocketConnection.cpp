#include "SocketConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
namespace Proxy
{

    SocketConnection::SocketConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept
            : Connection(sockfd, state, pipeline)
    {}

    Status SocketConnection::ReadData()
    {
        Status status;
        int32_t receivedData;
        uint8_t tmpBuffer[2048];
        signal(SIGPIPE, SIG_IGN);

        receivedData = recv(m_socket, tmpBuffer, sizeof(tmpBuffer), 0);
        if (receivedData < 0)
        {
            status = Status(Status::Error::BadRecievingDataFromSocket);
            return status;
        }
        if (receivedData == 0)
        {
            if(errno == EWOULDBLOCK)
            {
                status = Status(Status::Success::DataTransferEnded);
                return status;
            }
            status = Status(Status::Error::BadRecievingDataFromSocket);
            return status;
        }

        m_buffer.Insert(tmpBuffer, receivedData);

        return status;
    }

    Status SocketConnection::SendData(const ByteStream& data)
    {
        int32_t endpointSockfd;
        Status status;
        auto currentPipeline = m_pipeline.lock();
        if(!currentPipeline)
        {
            status = Status(Status::Error::BadGetAddrInfo);
        }
        if(m_socket == currentPipeline->GetClientSockfd())
        {
            endpointSockfd = currentPipeline->GetServerSockfd();
        }
        if(m_socket == currentPipeline->GetServerSockfd())
        {
            endpointSockfd = currentPipeline->GetClientSockfd();
        }

        auto onetimeDataSent = send(endpointSockfd, data.GetBuffer(), data.GetUsedBytes(),
                                    MSG_NOSIGNAL);
        if (onetimeDataSent == -1)
        {
            status = Status(Status::Error::BadSendingData);
            return status;
        }

        return status;
    }

    Status SocketConnection::SendDataTo(const ByteStream& data, SocketConnection& recipientConnection) noexcept
    {
        Status status;
        auto onetimeDataSent = send(recipientConnection.GetSocketfd(), data.GetBuffer(), data.GetUsedBytes(),
                                    MSG_NOSIGNAL);
        if (onetimeDataSent == -1)
        {
            status = Status(Status::Error::BadSendingData);
            return status;
        }

        return status;
    }
}