#ifndef PROXYTCP_TLSPARSER_HPP
#define PROXYTCP_TLSPARSER_HPP

#include <string>
#include <src/ByteStream/ByteStream.hpp>

namespace Proxy::TrafficParsing
{
    class TLSParser
    {
    public:
        static bool IsClientHelloMessage(const uint8_t* buff, int32_t offset = 0) noexcept;
        static ByteStream GetDomainNameFromTCPPacket(const uint8_t* buffer, uint32_t offset = 0) noexcept;

    private:
        static uint16_t ExtractUint16FromNetworkData(const uint8_t* buff) noexcept;
        static uint32_t ExtractUint32FromNetworkData(const uint8_t* buff) noexcept;

    };

}

#endif //PROXYTCP_TLSPARSER_HPP
