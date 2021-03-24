#include "SocketCapturingConnection.hpp"
#include "ConversationPipeline/CapturingConversationPipeline.hpp"
#include "PCAPGenerator/PCAPGenerator.hpp"

namespace Proxy
{
    SocketCapturingConnection::SocketCapturingConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept
            : SocketConnection(sockfd, state, pipeline)
    {}

    Status SocketCapturingConnection::ReadData()
    {
        Status status = SocketConnection::ReadData();
        if(status.Failed()) { return status; }

        const auto currentPipeline = dynamic_cast<CapturingConversationPipeline*>(m_pipeline.lock().get());
        if(!currentPipeline)
        {
            status = Status(Status::Error::NoPipelineFound);
            return status;
        }

        //if data was sended from client
        if(m_connectionSide == ConnectionSide::Client)
        {
            CaptureData(currentPipeline->PCAPFile(), m_buffer, currentPipeline->ClientPCAPData(), currentPipeline->ServerPCAPData());
        }
        //if data was sended from server
        else
        {
            CaptureData(currentPipeline->PCAPFile(), m_buffer, currentPipeline->ServerPCAPData(), currentPipeline->ClientPCAPData());
        }

        return status;
    }



    void SocketCapturingConnection::CaptureData(PCAP::PCAPCapturingFile& file, const ByteStream& tcpPayload, PCAPData& senderPCAPData, PCAPData& recipientPCAPData)
    {
        uint32_t sourceIPv4 = senderPCAPData.m_ipv4;
        uint16_t sourcePort = senderPCAPData.m_port;
        uint32_t destinationIPv4 = recipientPCAPData.m_ipv4;
        uint16_t destinationPort = recipientPCAPData.m_port;

        auto now = std::chrono::system_clock::now();
        auto pcapTimestamp = PCAP::PCAPGenerator::GeneratePCAPTimestampFromNow(now);

        senderPCAPData.m_timestamp.TSval = pcapTimestamp.TSsec;
        recipientPCAPData.m_timestamp.TSecr = pcapTimestamp.TSsec;


        file.Write(PCAP::PCAPGenerator::GeneratePCAPPacketHeader(tcpPayload.GetUsedBytes(), senderPCAPData.m_timestamp.TSval, pcapTimestamp.TSusec));
        file.Write(PCAP::PCAPGenerator::GenerateEthHeader());
        file.Write(PCAP::PCAPGenerator::GenerateIPv4Header(tcpPayload.GetUsedBytes(), sourceIPv4, destinationIPv4));
        file.Write(PCAP::PCAPGenerator::GenerateTCPHeader(senderPCAPData, recipientPCAPData , tcpPayload.GetUsedBytes(),
                                                          sourcePort, destinationPort, static_cast<uint16_t>(TCP::Flags::PSHACK)));
        file.Write(tcpPayload);

        file.Write(PCAP::PCAPGenerator::GenerateNoTCPPayloadPacket(recipientPCAPData, senderPCAPData, destinationIPv4, destinationPort,
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

        const auto currentPipeline = dynamic_cast<CapturingConversationPipeline*>(recipientConnection.Pipeline().lock().get());

        if(!currentPipeline)
        {
            status = Status(Status::Error::NoPipelineFound);
            return status;
        }

        if(currentPipeline->GetConversationFlowState() == ConversationFlow::FlowState::SOCKS5ClientHello ||
           currentPipeline->GetConversationFlowState() == ConversationFlow::FlowState::SOCKS5ConnectionRequest)
        {
            //if data was sended from server
            if(recipientConnection.GetConnectionSide() == ConnectionSide::Client)
            {
                CaptureData(currentPipeline->PCAPFile(), data, currentPipeline->ServerPCAPData(), currentPipeline->ClientPCAPData());
            }
            //if data was sended from client
            else
            {
                CaptureData(currentPipeline->PCAPFile(), data, currentPipeline->ClientPCAPData(), currentPipeline->ServerPCAPData());
            }
        }

        return status;
    }

}