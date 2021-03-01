#ifndef PROXYTCP_PCAPPARSER_HPP
#define PROXYTCP_PCAPPARSER_HPP

#include "Utilities/Constants.hpp"
#include "ByteStream/ByteStream.hpp"

namespace Proxy::TrafficParsing
{
    class PCAPParser
    {
    public:
        static uint32_t GetPacketSize(const uint8_t* data, std::size_t dataSize) noexcept;

    public:
        static ByteStream GenerateEthHeader() noexcept;
        static ByteStream GeneratePCAPPacketHeader(uint32_t tcpPayloadSize) noexcept;
        static ByteStream GeneratePCAPGlobalHeader() noexcept;
        static ByteStream GenerateIPv4Header(uint16_t tcpPayloadSize, uint32_t sourceIPv4, uint32_t desinationIPv4) noexcept;
        static ByteStream GenerateTCPHeader(uint32_t sequenceNumber, uint32_t sourcePort, uint32_t destinationPort) noexcept;

    public:
        static uint32_t TCPSequenceNumber;
        static const uint32_t ServerIPv4;
        static const uint32_t ClientIPv4;
        static const uint16_t ServerPort;
        static const uint16_t ClientPort;

    };

}



#endif //PROXYTCP_PCAPPARSER_HPP
