#include <cstring>
#include <netinet/in.h>
#include <src/Utilities/Constants.hpp>
#include <src/Status.hpp>
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

    bool TLSMessageParser::AllMessagesArrived(const uint8_t* tlsRecordPayload, uint32_t tlsRecordPayloadSize)
    {
        uint32_t processedBytes = 0;

        auto firstTLSMessageSize = GetTLSMessageSize(tlsRecordPayload + 1, tlsRecordPayloadSize);

        processedBytes += firstTLSMessageSize + HeaderSize::TLS_MESSAGE;

        if(firstTLSMessageSize == tlsRecordPayloadSize - HeaderSize::TLS_MESSAGE) { return true; }
        if(firstTLSMessageSize <  tlsRecordPayloadSize - HeaderSize::TLS_MESSAGE)
        {
            while(processedBytes < tlsRecordPayloadSize)
            {
                auto newTLSMessageSize = GetTLSMessageSize(tlsRecordPayload + processedBytes + 1, tlsRecordPayloadSize - processedBytes);

                processedBytes += newTLSMessageSize + HeaderSize::TLS_MESSAGE;
            }

            if(processedBytes == tlsRecordPayloadSize) { return true; }
            if(processedBytes >  tlsRecordPayloadSize ) { return false; }

        }
        return false;
    }
}
