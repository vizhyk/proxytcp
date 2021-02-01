#ifndef PROXYTCP_TLSMESSAGEPARSER_HPP
#define PROXYTCP_TLSMESSAGEPARSER_HPP

#include <cstdint>

namespace Proxy::TrafficParsing
{
    class TLSMessageParser
    {
    public:
        static uint32_t GetTLSMessageSize(const uint8_t* buffer, std::size_t bufferSize) noexcept;
    };
}

#endif //PROXYTCP_TLSMESSAGEPARSER_HPP
