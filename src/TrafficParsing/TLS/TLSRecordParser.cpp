#include <cstring>
#include <netinet/in.h>
#include "TLSRecordParser.hpp"
#include "src/Utilities/Constants.hpp"
#include "TLSMessageParser.hpp"

namespace Proxy::TrafficParsing
{
    uint16_t TLSRecordParser::GetTLSRecordPayloadSize(const uint8_t* buffer, std::size_t bufferSize) noexcept
    {
        uint16_t recordSize;
        if(bufferSize < 5)
        {
            return 0;
        }

        memcpy(&recordSize, buffer + 3, sizeof(recordSize));

        return ntohs(recordSize);

    }

    Status TLSRecordParser::IsRecordFull(const uint8_t* tlsRecordData, uint32_t tlsRecordSize) noexcept
    {
        using namespace Utilities;
        const uint8_t contentType = tlsRecordData[0];

        auto payloadSize = GetTLSRecordPayloadSize(tlsRecordData,tlsRecordSize);

        if(payloadSize == tlsRecordSize - HeaderSize::TLS_RECORD)
            return Status(Status::Success::Success);

        return Status(Status::Success::WaitingForData);

    }

}

