#ifndef PROXYTCP_SOCKS5PARSER_HPP
#define PROXYTCP_SOCKS5PARSER_HPP

#include "src/ByteStream/ByteStream.hpp"

namespace Proxy::TrafficParsing
{
    class SOCKS5Parser
    {
    public:
        static bool IsValidConnectionRequestMessage(const uint8_t* buffer, int32_t bufferSize) noexcept;
        static bool IsValidClientHelloMessage(const uint8_t* buffer, int32_t bufferSize) noexcept;
        static uint8_t GetClientAuthenticationMethod(const uint8_t* buffer, uint32_t bufferSize) noexcept;
        static uint8_t GetDestinationAddressType(const uint8_t* buffer, uint32_t bufferSize) noexcept;
        static Status GetDestinationAddressAndPort(const uint8_t* buffer, uint32_t bufferSize, ByteStream& destinationAddress, uint16_t& port) noexcept;
        static int32_t GetConnectionRequestLength(const uint8_t* buffer, size_t buffersize);

    private:
        static void MemcpyDestinationAddressAndPortFromBuffer(const uint8_t* buffer, uint8_t destinationAddressSize, ByteStream& stream, uint16_t& port, uint8_t offset) noexcept;
    };
}



#endif //PROXYTCP_SOCKS5PARSER_HPP
