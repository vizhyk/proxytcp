#include "SocketCapturingConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "TrafficParsing/PCAP/PCAPParser.hpp"
namespace Proxy
{
    SocketCapturingConnection::SocketCapturingConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept
            : SocketConnection(sockfd, state, pipeline)
    {}

    Status SocketCapturingConnection::ReadData()
    {
        Status status;
        int32_t receivedData;
        uint8_t tmpBuffer[2048];
        signal(SIGPIPE, SIG_IGN);

        receivedData = recv(m_socket, tmpBuffer, sizeof(tmpBuffer), 0);

        if (receivedData == -1)
        {
            status = Status(Status::Error::BadRecievingDataFromSocket);
            return status;
        }

        if (receivedData == 0)
        {
            status = Status(Status::Success::Success);
            return status;
        }

        m_buffer.Insert(tmpBuffer, receivedData);

        auto currentPipeline = m_pipeline.lock();
        if(!currentPipeline)
        {
            status = Status(Status::Error::BadGetAddrInfo);
        }

        CaptureData(currentPipeline->PCAPFile());

        return status;
    }

//    Status SocketCapturingConnection::SendData(const ByteStream& data)
//    {
//        int32_t endpointSockfd;
//        Status status;
//        auto currentPipeline = m_pipeline.lock();
//        if(!currentPipeline)
//        {
//            status = Status(Status::Error::BadGetAddrInfo);
//        }
//        if(m_socket == currentPipeline->GetClientSockfd())
//        {
//            endpointSockfd = currentPipeline->GetServerSockfd();
//        }
//        if(m_socket == currentPipeline->GetServerSockfd())
//        {
//            endpointSockfd = currentPipeline->GetClientSockfd();
//        }
//
//        auto onetimeDataSent = send(endpointSockfd, data.GetBuffer(), data.GetUsedBytes(),
//                                    MSG_NOSIGNAL);
//        if (onetimeDataSent == -1)
//        {
//            status = Status(Status::Error::BadSendingData);
//            return status;
//        }
//
//        return status;
//    }

    void SocketCapturingConnection::CaptureData(PCAP::PCAPCapturingFile& file)
    {
        uint32_t sourceIPv4 = 0;
        uint32_t destinationIPv4 = 0;
        uint16_t sourcePort = 0;
        uint16_t destinationPort = 0;

        if(m_connectionSide == ConnectionSide::Server)
        {
            sourceIPv4 = TrafficParsing::PCAPParser::ServerIPv4;
            sourcePort = TrafficParsing::PCAPParser::ServerPort;
            destinationIPv4 = TrafficParsing::PCAPParser::ClientIPv4;
            destinationPort = TrafficParsing::PCAPParser::ClientPort;

        }
        else
        {
            sourceIPv4 = TrafficParsing::PCAPParser::ClientIPv4;
            sourcePort = TrafficParsing::PCAPParser::ClientPort;
            destinationIPv4 = TrafficParsing::PCAPParser::ServerIPv4;
            destinationPort = TrafficParsing::PCAPParser::ServerPort;
        }

        file.Write(TrafficParsing::PCAPParser::GeneratePCAPPacketHeader(m_buffer.GetUsedBytes()));
        file.Write(TrafficParsing::PCAPParser::GenerateEthHeader());
        file.Write(TrafficParsing::PCAPParser::GenerateIPv4Header(m_buffer.GetUsedBytes(), sourceIPv4, destinationIPv4));
        file.Write(TrafficParsing::PCAPParser::GenerateTCPHeader(TrafficParsing::PCAPParser::TCPSequenceNumber, sourcePort, destinationPort));
        file.Write(m_buffer);

        TrafficParsing::PCAPParser::TCPSequenceNumber++;
    }


}