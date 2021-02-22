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
        static ByteStream GeneratePCAPPacketHeader(uint32_t packetSize) noexcept;
        static ByteStream GeneratePCAPGlobalHeader() noexcept;
        static ByteStream GenerateIPv4Header() noexcept;
        static ByteStream GenerateTCPHeader() noexcept;

    };
}



#endif //PROXYTCP_PCAPPARSER_HPP
