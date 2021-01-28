#include <cstring>
#include <netinet/in.h>
#include "TLSRecordParser.hpp"
#include "TLSMessageParser.hpp"
#include "src/Utilities/Constants.hpp"

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

//        if(contentType == Utilities::TLS::ContentType::Handshake)
//        {
//            auto allMessagesArrived = TLSMessageParser::AllMessagesArrived(tlsRecordData+HeaderSize::TLS_RECORD, tlsRecordSize - HeaderSize::TLS_RECORD);
//
//            if(allMessagesArrived)
//                return Status(Status::Success::Success);
//
//            return Status(Status::Success::WaitingForTLSMessages);
//        }

//        if(contentType == TLS::ContentType::ApplicationData || contentType == TLS::ContentType::ChangeCipherSpec)
//        {
        auto payloadSize = GetTLSRecordPayloadSize(tlsRecordData,tlsRecordSize);

        if(payloadSize == tlsRecordSize - HeaderSize::TLS_RECORD)
            return Status(Status::Success::Success);

        return Status(Status::Success::WaitingForData);
//        }

        return Status(Status::Error::BadTLSRecordContentType);
    }
}

