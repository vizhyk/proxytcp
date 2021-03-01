#include <netinet/in.h>

#include "PCAPParser.hpp"
namespace Proxy::TrafficParsing
{
    uint32_t PCAPParser::TCPSequenceNumber = 0;
    uint32_t const PCAPParser::ServerIPv4 = 0x0b0b0b0b;
    uint32_t const PCAPParser::ClientIPv4 = 0x16161616;
    uint16_t const PCAPParser::ServerPort = 57987;
    uint16_t const PCAPParser::ClientPort = 443;



    uint32_t PCAPParser::GetPacketSize(const uint8_t* data, std::size_t dataSize) noexcept
    {
        if(dataSize < 16)
            return 0;

        uint32_t packetSize = 0;

        memcpy(&packetSize,data + 12,dataSize);

        return packetSize;
    }

    ByteStream PCAPParser::GenerateEthHeader() noexcept
    {
        const uint8_t tmpMacAdresses[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        ByteStream tmpEthHeader(HeaderSize::ETH);
        tmpEthHeader.Insert(tmpMacAdresses, sizeof(tmpMacAdresses));
        tmpEthHeader.Insert(static_cast<uint16_t>(htons(0x0800)));

        //TODO: assert

        return tmpEthHeader;
    }

    ByteStream PCAPParser::GeneratePCAPGlobalHeader() noexcept
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

    ByteStream PCAPParser::GeneratePCAPPacketHeader(uint32_t tcpPayloadSize) noexcept
    {

        ByteStream tmpPCAPPacketHeader(HeaderSize::PCAP_PACKET_HEADER);

        uint32_t networkPacketSize = tcpPayloadSize + HeaderSize::TCP + HeaderSize::IP + HeaderSize::ETH;

        //timestamp seconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(0x00));
        //timestamp microseconds
        tmpPCAPPacketHeader.Insert(static_cast<uint32_t>(0x00));
        //bytes saved in file
        tmpPCAPPacketHeader.Insert(networkPacketSize);
        //actual length of packet
        tmpPCAPPacketHeader.Insert(networkPacketSize);

        //TODO: static assertion
        return tmpPCAPPacketHeader;
    }

    ByteStream PCAPParser::GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4) noexcept
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
        tmpIPv4Header.Insert(static_cast<uint32_t>(sourceIPv4));
        //ipv4 dst
        tmpIPv4Header.Insert(static_cast<uint32_t>(desinationIPv4));

        //TODO: static assertion
        if(tmpIPv4Header.GetUsedBytes() != HeaderSize::IP)
        {
            exit(1);
        }

        return tmpIPv4Header;
    }

    ByteStream PCAPParser::GenerateTCPHeader(uint32_t sequenceNumber, uint32_t sourcePort, uint32_t destinationPort) noexcept
    {
        ByteStream tmpTCPHeader(HeaderSize::TCP);

        //source port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(sourcePort)));
        //destination port
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(destinationPort)));
        //sequence number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(sequenceNumber)));
        //acknowledgment number
        tmpTCPHeader.Insert(static_cast<uint32_t>(htonl(sequenceNumber)));
        //flags
        tmpTCPHeader.Insert(static_cast<uint16_t>(0x80));
        //window size
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(2048)));
        //checksum
        tmpTCPHeader.Insert(static_cast<uint16_t>(htons(0x8025)));
        //urgent pointer
        tmpTCPHeader.Insert(static_cast<uint16_t>(0x0000));
        //options + padding
        //options::NO-OP
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));
        //options::NO-OP
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x01));
        //options::timestamp option
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x08));
        //options::timestamp length
        tmpTCPHeader.Insert(static_cast<uint8_t>(0x0a));
        //options::timestamp value
        tmpTCPHeader.Insert(static_cast<uint32_t>(0x11223344));
        //options::timestamp echo
        tmpTCPHeader.Insert(static_cast<uint32_t>(0x22334455));

        return tmpTCPHeader;
    }
}