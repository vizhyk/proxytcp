#include <netinet/in.h>
#include <iostream>
#include "PCAPGenerator.hpp"

namespace Proxy::PCAP
{
    const DefaultEndpoints PCAPGenerator::defaultEndpoints{{0x0b0b0b0b, 1080}, {0x16161616, 39510}};
    uint32_t PCAPGenerator::pipelinesCount = 0;

    uint32_t PCAPGenerator::GetPacketSize(const uint8_t* data, std::size_t dataSize) noexcept
    {
        if(dataSize < 16)
            return 0;

        uint32_t packetSize = 0;

        memcpy(&packetSize,data + 12,dataSize);

        return packetSize;
    }

    ByteStream PCAPGenerator::GeneratePCAPGlobalHeader() noexcept
    {
        ByteStream tmpPCAPGlobalHeader;

        //magic number
        tmpPCAPGlobalHeader.Insert(static_cast<uint32_t>(0xa1b2c3d4));
        // major version
        tmpPCAPGlobalHeader.Insert(static_cast<uint16_t>(0x0002));
        // minor version
        tmpPCAPGlobalHeader.Insert(static_cast<uint16_t>(0x0004));
        //thiszone : correction in seconds from UTC
        tmpPCAPGlobalHeader.Insert(static_cast<uint32_t>(0x00));
        //accuracy of timestamps
        tmpPCAPGlobalHeader.Insert(static_cast<uint32_t>(0x00));
        //max length of packet (65535)
        tmpPCAPGlobalHeader.Insert(static_cast<uint32_t>(65535));
        //type of data link layer(1 == ethernet)
        tmpPCAPGlobalHeader.Insert(static_cast<uint32_t>(0x01));

        return tmpPCAPGlobalHeader;
    }

    ByteStream PCAPGenerator::GeneratePCAPPacketHeader(uint32_t tcpPayloadSize, uint32_t TSsec, uint32_t TSusec) noexcept
    {

        ByteStream tmpPCAPPacketHeader(HeaderSize::PCAP_PACKET_HEADER);

        uint32_t networkPacketSize = tcpPayloadSize + HeaderSize::TCP + HeaderSize::IP + HeaderSize::ETH;

        //timestamp seconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(TSsec));
        //timestamp microseconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(TSusec));
        //bytes saved in file
        tmpPCAPPacketHeader.Insert(networkPacketSize);
        //actual length of packet
        tmpPCAPPacketHeader.Insert(networkPacketSize);

        //TODO: static assertion
        return tmpPCAPPacketHeader;
    }

    ByteStream PCAPGenerator::GeneratePCAPPacketHeader(uint32_t tcpPayloadSize, uint16_t TCPHeaderSize, uint32_t TSsec, uint32_t TSusec) noexcept
    {
        ByteStream tmpPCAPPacketHeader(HeaderSize::PCAP_PACKET_HEADER);

        uint32_t networkPacketSize = tcpPayloadSize + TCPHeaderSize + HeaderSize::IP + HeaderSize::ETH;

        //timestamp seconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(TSsec));
        //timestamp microseconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(TSusec));
        //bytes saved in file
        tmpPCAPPacketHeader.Insert(networkPacketSize);
        //actual length of packet
        tmpPCAPPacketHeader.Insert(networkPacketSize);

        //TODO: static assertion
        return tmpPCAPPacketHeader;
    }

    ByteStream PCAPGenerator::GenerateEthHeader() noexcept
    {
        const uint8_t tmpMacAdresses[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        ByteStream tmpEthHeader(HeaderSize::ETH);
        tmpEthHeader.Insert(tmpMacAdresses, sizeof(tmpMacAdresses));
        tmpEthHeader.Insert(static_cast<uint16_t>(htons(0x0800)));

        //TODO: assert

        return tmpEthHeader;
    }

    ByteStream PCAPGenerator::GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4) noexcept
    {
        auto IPv4PacketSize = htons(tcpPayloadSize + HeaderSize::TCP + HeaderSize::IP);

        ByteStream tmpIPv4Header(HeaderSize::IP);

        //protocol version & header size
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x45));
        //differentiated services
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x00));
        //total length
        tmpIPv4Header.Insert(static_cast<uint16_t>(IPv4PacketSize));
        //identification
        tmpIPv4Header.Insert(static_cast<uint16_t>(0x0000));
        //flags & fragment offset
        tmpIPv4Header.Insert(static_cast<uint16_t>(htons(0x4000)));
        //TTL
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x40));
        //protocol
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x06));
        //checksum
        tmpIPv4Header.Insert(static_cast<uint16_t>(0x1111));
        //ipv4 src
        tmpIPv4Header.Insert(static_cast<uint32_t>(htonl(sourceIPv4)));
        //ipv4 dst
        tmpIPv4Header.Insert(static_cast<uint32_t>(htonl(desinationIPv4)));

        //TODO: static assertion
        if(tmpIPv4Header.GetUsedBytes() != HeaderSize::IP)
        {
            exit(1);
        }

        return tmpIPv4Header;
    }

    ByteStream PCAPGenerator::GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4, uint16_t TCPHeaderSize) noexcept
    {
        auto IPv4PacketSize = htons(tcpPayloadSize + TCPHeaderSize + HeaderSize::IP);

        ByteStream tmpIPv4Header(HeaderSize::IP);

        //protocol version & header size
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x45));
        //differentiated services
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x00));
        //total length
        tmpIPv4Header.Insert(static_cast<uint16_t>(IPv4PacketSize));
        //identification
        tmpIPv4Header.Insert(static_cast<uint16_t>(0x0000));
        //flags & fragment offset
        tmpIPv4Header.Insert(static_cast<uint16_t>(htons(0x4000)));
        //TTL
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x40));
        //protocol
        tmpIPv4Header.Insert(static_cast<uint8_t>(0x06));
        //checksum
        tmpIPv4Header.Insert(static_cast<uint16_t>(0x1111));
        //ipv4 src
        tmpIPv4Header.Insert(static_cast<uint32_t>(htonl(sourceIPv4)));
        //ipv4 dst
        tmpIPv4Header.Insert(static_cast<uint32_t>(htonl(desinationIPv4)));

        //TODO: static assertion
        if(tmpIPv4Header.GetUsedBytes() != HeaderSize::IP)
        {
            exit(1);
        }

        return tmpIPv4Header;
    }

    ByteStream PCAPGenerator::GenerateTCPHeader(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t tcpPayloadSize, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags, uint32_t TSval, uint32_t TSecr) noexcept
    {
        ByteStream tmpTCPHeader(HeaderSize::TCP);

        //source port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(sourcePort)));
        //destination port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(destinationPort)));
        //sequence number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(sequenceNumber)));
        //acknowledgment number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(acknowledgmentNumber)));
        //flags
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8000 + flags)));
        //window size
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(2048)));
        //checksum
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8025)));
        //urgent pointer
        tmpTCPHeader.Insert(static_cast<uint16_t>(0x0000));

        //options + padding

        //option::NO-OP
        //option::NO-OP::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));
        //option::NO-OP
        //option::NO-OP::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));

        //option::Timestamps
        //option::Timestamps::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x08));
        //option::Timestamps::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x0a));
        //option::Timestamps::Value
        tmpTCPHeader.Insert(static_cast<uint32_t>(TSval));
        //option::Timestamps::Echo reply
        tmpTCPHeader.Insert(static_cast<uint32_t>(TSecr));

        return tmpTCPHeader;
    }

    ByteStream PCAPGenerator::GenerateTCPHeader(PCAPData& senderPCAPData, PCAPData& recipientPCAPData, uint32_t tcpPayloadSize, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags) noexcept
    {
        ByteStream tmpTCPHeader(HeaderSize::TCP);

        //source port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(sourcePort)));
        //destination port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(destinationPort)));
        //sequence number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(senderPCAPData.m_sequenceNumber)));
        //acknowledgment number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(senderPCAPData.m_acknowledgmentNumber)));
        //flags
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8000 + flags)));
        //window size
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(2048)));
        //checksum
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8025)));
        //urgent pointer
        tmpTCPHeader.Insert(static_cast<uint16_t>(0x0000));

        //options + padding

        //option::NO-OP
        //option::NO-OP::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));
        //option::NO-OP
        //option::NO-OP::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));

        //option::Timestamps
        //option::Timestamps::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x08));
        //option::Timestamps::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x0a));
        //option::Timestamps::Value
        tmpTCPHeader.Insert(static_cast<uint32_t>(senderPCAPData.m_timestamp.TSval));
        //option::Timestamps::Echo reply
        tmpTCPHeader.Insert(static_cast<uint32_t>(senderPCAPData.m_timestamp.TSval));

        senderPCAPData.m_sequenceNumber += tcpPayloadSize;
        recipientPCAPData.m_acknowledgmentNumber += tcpPayloadSize;

        return tmpTCPHeader;
    }

    ByteStream PCAPGenerator::GenerateTCPHeaderSYNACKOptions(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t sourcePort, uint32_t destinationPort, uint16_t flags, uint32_t TSval, uint32_t TSecr) noexcept
    {
        ByteStream tmpTCPHeader(HeaderSize::TCP);

        //source port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(sourcePort)));
        //destination port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(destinationPort)));
        //sequence number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(sequenceNumber)));
        //acknowledgment number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(acknowledgmentNumber)));
        //flags
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0xa000 + flags)));
        //window size
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(2048)));
        //checksum
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8025)));
        //urgent pointer
        tmpTCPHeader.Insert(static_cast<uint16_t>(0x0000));

        //options + padding

        //option::Maximum segment size
        //option::Maximum segment size::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x02));
        //option::Maximum segment size::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x04));
        //option::Maximum segment size::MSS Value
        tmpTCPHeader.Insert(static_cast<uint16_t>(ntohs(0xffd7)));

        //option::SACK Permitted
        //option::SACK Permitted::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x04));
        //option::SACK Permitted::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x02));

        //option::Timestamps
        //option::Timestamps::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x08));
        //option::Timestamps::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x0a));
        //option::Timestamps::Value
        tmpTCPHeader.Insert(static_cast<uint32_t>(TSval));
        //option::Timestamps::Echo reply
        tmpTCPHeader.Insert(static_cast<uint32_t>(TSecr));

        //option::NO-OP
        //option::NO-OP::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));

        //option::Window Scale
        //option::Window Scale::Kind
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x03));
        //option::Window Scale::Length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x03));
        //option::Window Scale::Shift Count
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x07));
        // window scale = pow(2,0x07) = 128

        return tmpTCPHeader;
    }

    ByteStream PCAPGenerator::GenerateNoTCPPayloadPacket(PCAPData& lhsPCAPData, PCAPData& rhsPCAPData, uint32_t sourceIPv4, uint16_t sourcePort, uint32_t destinationIPv4, uint16_t destinationPort, uint16_t flags) noexcept
    {
        ByteStream tmpAckPacket;

        auto now = std::chrono::system_clock::now();
        auto pcapTimestamp = GeneratePCAPTimestampFromNow(now);

        lhsPCAPData.m_timestamp.TSval = pcapTimestamp.TSsec;
        rhsPCAPData.m_timestamp.TSecr = pcapTimestamp.TSsec;


        tmpAckPacket.Insert(GeneratePCAPPacketHeader(0, lhsPCAPData.m_timestamp.TSval, pcapTimestamp.TSusec));
        tmpAckPacket.Insert( GenerateEthHeader());
        tmpAckPacket.Insert( GenerateIPv4Header(0,sourceIPv4,destinationIPv4));
        tmpAckPacket.Insert( GenerateTCPHeader(lhsPCAPData, rhsPCAPData, 0, sourcePort, destinationPort, flags));

        return tmpAckPacket;
    }

    ByteStream PCAPGenerator::Generate3WayTCPHandshake(PCAPData& client, PCAPData& server) noexcept
    {
        ByteStream tmp3WayHandshake;

        auto now = std::chrono::system_clock::now();
        auto pcapTimestamp = GeneratePCAPTimestampFromNow(now);

        client.m_timestamp.TSval = pcapTimestamp.TSsec;
        server.m_timestamp.TSecr = pcapTimestamp.TSsec;

        //SYN
        tmp3WayHandshake.Insert(GeneratePCAPPacketHeader(0, HeaderSize::TCP_SYNACK, client.m_timestamp.TSval, pcapTimestamp.TSusec));
        tmp3WayHandshake.Insert(GenerateEthHeader());
        tmp3WayHandshake.Insert(GenerateIPv4Header(0,client.m_ipv4,server.m_ipv4, HeaderSize::TCP_SYNACK));
        tmp3WayHandshake.Insert(GenerateTCPHeaderSYNACKOptions(0, 0, client.m_port, server.m_port, static_cast<uint16_t>(TCP::Flags::SYN), client.m_timestamp.TSval, client.m_timestamp.TSecr));

        now = std::chrono::system_clock::now();
        pcapTimestamp = GeneratePCAPTimestampFromNow(now);

        server.m_timestamp.TSval = pcapTimestamp.TSsec;
        client.m_timestamp.TSecr = pcapTimestamp.TSsec;

        //SYNACK
        tmp3WayHandshake.Insert(GeneratePCAPPacketHeader(0, HeaderSize::TCP_SYNACK, server.m_timestamp.TSval, pcapTimestamp.TSusec));
        tmp3WayHandshake.Insert(GenerateEthHeader());
        tmp3WayHandshake.Insert(GenerateIPv4Header(0,server.m_ipv4,client.m_ipv4, HeaderSize::TCP_SYNACK));
        tmp3WayHandshake.Insert(GenerateTCPHeaderSYNACKOptions(0, 1, server.m_ipv4, client.m_port, static_cast<uint16_t>(TCP::Flags::SYNACK), server.m_timestamp.TSval, server.m_timestamp.TSecr));

        now = std::chrono::system_clock::now();
        pcapTimestamp = GeneratePCAPTimestampFromNow(now);

        client.m_timestamp.TSval = pcapTimestamp.TSsec;
        server.m_timestamp.TSecr = pcapTimestamp.TSsec;

        //ACK
        tmp3WayHandshake.Insert(GeneratePCAPPacketHeader(0, client.m_timestamp.TSval, pcapTimestamp.TSusec));
        tmp3WayHandshake.Insert(GenerateEthHeader());
        tmp3WayHandshake.Insert(GenerateIPv4Header(0,client.m_ipv4,server.m_ipv4, HeaderSize::TCP));
        tmp3WayHandshake.Insert(GenerateTCPHeader(1, 1, 0, client.m_port, server.m_port, static_cast<uint16_t>(TCP::Flags::ACK), client.m_timestamp.TSval, client.m_timestamp.TSecr));

        return  tmp3WayHandshake;
    }

    DefaultEndpoints PCAPGenerator::GenerateNewPipelineEndpoints() noexcept
    {
        DefaultEndpoints tmp = defaultEndpoints;
        tmp.server.ipv4 += pipelinesCount;
        tmp.client.ipv4 += pipelinesCount;
        tmp.client.port += pipelinesCount;
        //server port == 1080 by default
        pipelinesCount += 1;
        return tmp;
    }

    uint32_t PCAPGenerator::ChronoNowSecondsUint32_t() noexcept
    {
        auto now = std::chrono::system_clock::now();

        return std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch().count();
    }

    PCAPTimestamp PCAPGenerator::GeneratePCAPTimestampFromNow(const std::chrono::time_point<std::chrono::system_clock>& now)
    {
        uint32_t second = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        uint32_t microsecond = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count() - second * 1'000'000;
        return {second,microsecond};
    }

}