#include <netinet/in.h>

#include "PCAPParser.hpp"
namespace Proxy::TrafficParsing
{

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
        const uint8_t tmpEth[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00};

        return ByteStream(tmpEth, sizeof(tmpEth));
    }

    ByteStream PCAPParser::GeneratePCAPGlobalHeader() noexcept
    {
        const uint8_t tmpPCAPGlobalHeader[] = {0xc3, 0xd4, 0xa1, 0xb2, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x01, 0x00, 0x00};

        return ByteStream(tmpPCAPGlobalHeader,sizeof(tmpPCAPGlobalHeader));
    }

    ByteStream PCAPParser::GeneratePCAPPacketHeader(uint32_t packetSize) noexcept
    {
        const uint8_t headerTimestamps[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

        ByteStream tmpPCAPPacketHeader(headerTimestamps,sizeof(headerTimestamps));

        auto networkPacketSize = htonl(packetSize);

        //bytes saved in file
        tmpPCAPPacketHeader.Insert(networkPacketSize);
        //actual length of packet
        tmpPCAPPacketHeader.Insert(networkPacketSize);

        return tmpPCAPPacketHeader;
    }
}