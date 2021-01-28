#ifndef PROXYTCP_TLSPARSER_HPP
#define PROXYTCP_TLSPARSER_HPP

#include <string>
#include <src/ByteStream/ByteStream.hpp>
#include "TLSRecordParser.hpp"
#include "TLSMessageParser.hpp"

namespace Proxy::TrafficParsing
{
    class TLSParser
    {
    public:
        static uint16_t GetTLSRecordPayloadSize(const uint8_t* buffer, std::size_t bufferSize) noexcept;
        static uint32_t GetTLSMessageSize(const uint8_t* buffer, std::size_t bufferSize) noexcept;
        static bool IsClientHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept;
        static bool IsServerHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept;
        static Status GetDomainNameFromTLSPacket(const uint8_t* buffer, uint32_t bufferSize, uint32_t offset, ByteStream& emptyByteStream) noexcept;
        static Status TLSPacketArrivedFully(const uint8_t* buffer, std::size_t bufferSize) noexcept;
        static uint16_t ExtractUint16FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept;
        static uint32_t ExtractUint32FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept;
        static Status ExtractAllValidRecords(const uint8_t* buffer, uint32_t bufferSize, ByteStream& validRecords, ByteStream& incompleteTLSRecords) noexcept;
        static Status CopyEveryRecordPayloadToMessageBuffer(const ByteStream& connectionBuffer, ByteStream& messageData) noexcept;
        static Status CopyEveryFulLRecordPayloadToMessageBuffer(const ByteStream& recordBuffer, ByteStream& messageBuffer) noexcept;
        static Status ProcessEveryFullMessagAndGenerateRecords(ByteStream& messageBuffer, ByteStream& emptyRecordBuffer) noexcept;
        static Status FindFullServerHelloRecord(const ByteStream& connectionBuffer, ByteStream& recordBuffer, ByteStream& unprocessedRecords) noexcept;
    };

}

#endif //PROXYTCP_TLSPARSER_HPP
