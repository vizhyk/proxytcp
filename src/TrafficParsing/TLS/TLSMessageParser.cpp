#include <cstring>
#include <netinet/in.h>
#include <src/Utilities/Constants.hpp>
#include "TLSMessageParser.hpp"

namespace Proxy::TrafficParsing
{
    uint32_t TLSMessageParser::GetTLSMessageSize(const uint8_t* buffer, std::size_t bufferSize) noexcept
    {
        uint32_t messageSize = 0;

        if(bufferSize < 3) { return 0; }

        memcpy(&messageSize, buffer, 3);
        messageSize = ntohl(messageSize);
        messageSize = messageSize >> 8;

        return messageSize;
    }

}
