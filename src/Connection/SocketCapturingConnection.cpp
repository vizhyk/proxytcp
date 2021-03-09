#include "SocketCapturingConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"

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
            status = Status(Status::Error::NoPipelineFound);
        }

        //if data was sended from client
        if(m_connectionSide == ConnectionSide::Client)
        {
            CaptureData(currentPipeline->PCAPFile(), m_buffer, currentPipeline->ClientSYNACK(), currentPipeline->ServerSYNACK(), ConnectionSide::Client);
        }
            //if data was sended from server
        else
        {
            CaptureData(currentPipeline->PCAPFile(), m_buffer, currentPipeline->ServerSYNACK(), currentPipeline->ClientSYNACK(), ConnectionSide::Server);
        }

        return status;
    }



    void SocketCapturingConnection::CaptureData(PCAP::PCAPCapturingFile& file, const ByteStream& data, SYNACKData& senderSYNACKData, SYNACKData& recipientSYNACKData, ConnectionSide senderConnectionSide)
    {
        uint32_t sourceIPv4 = 0;
        uint32_t destinationIPv4 = 0;
        uint16_t sourcePort = 0;
        uint16_t destinationPort = 0;

        if(senderConnectionSide == ConnectionSide::Server)
        {
            sourceIPv4 = PCAP::PCAPGenerator::defaultEndpoints.server.ipv4;
            sourcePort = PCAP::PCAPGenerator::defaultEndpoints.server.port;
            destinationIPv4 = PCAP::PCAPGenerator::defaultEndpoints.client.ipv4;
            destinationPort = PCAP::PCAPGenerator::defaultEndpoints.client.port;
        }
        else
        {
            sourceIPv4 = PCAP::PCAPGenerator::defaultEndpoints.client.ipv4;
            sourcePort = PCAP::PCAPGenerator::defaultEndpoints.client.port;
            destinationIPv4 = PCAP::PCAPGenerator::defaultEndpoints.server.ipv4;
            destinationPort = PCAP::PCAPGenerator::defaultEndpoints.server.port;
        }

        file.Write(PCAP::PCAPGenerator::GeneratePCAPPacketHeader(data.GetUsedBytes()));
        file.Write(PCAP::PCAPGenerator::GenerateEthHeader());
        file.Write(PCAP::PCAPGenerator::GenerateIPv4Header(data.GetUsedBytes(), sourceIPv4, destinationIPv4));
        file.Write(PCAP::PCAPGenerator::GenerateTCPHeader(senderSYNACKData, recipientSYNACKData , data.GetUsedBytes(),
                                                                    sourcePort, destinationPort, static_cast<uint16_t>(TCP::Flags::PSHACK)));
        file.Write(data);

        file.Write(PCAP::PCAPGenerator::GenerateNoTCPPayloadPacket(recipientSYNACKData, senderSYNACKData, destinationIPv4, destinationPort,
                                                                             sourceIPv4, sourcePort, static_cast<uint16_t>(TCP::Flags::ACK)));

    }

    Status SocketCapturingConnection::SendDataTo(const ByteStream& data, SocketConnection& recipientConnection) noexcept
    {
        Status status;
        auto onetimeDataSent = send(recipientConnection.GetSocketfd(), data.GetBuffer(), data.GetUsedBytes(),
                                    MSG_NOSIGNAL);
        if (onetimeDataSent == -1)
        {
            status = Status(Status::Error::BadSendingData);
            return status;
        }

        auto currentPipeline = recipientConnection.Pipeline().lock();
        if(!currentPipeline)
        {
            status = Status(Status::Error::NoPipelineFound);
        }

        if(currentPipeline->GetConversationFlowState() == ConversationFlow::FlowState::SOCKS5ClientHello ||
           currentPipeline->GetConversationFlowState() == ConversationFlow::FlowState::SOCKS5ConnectionRequest)
        {
            //if data was sended from server
            if(recipientConnection.GetConnectionSide() == ConnectionSide::Client)
            {
                CaptureData(currentPipeline->PCAPFile(), data, currentPipeline->ServerSYNACK(), currentPipeline->ClientSYNACK(), ConnectionSide::Server);
            }
            //if data was sended from client
            else
            {
                CaptureData(currentPipeline->PCAPFile(), data, currentPipeline->ClientSYNACK(), currentPipeline->ServerSYNACK(), ConnectionSide::Client);
            }
        }

        return status;
    }


}