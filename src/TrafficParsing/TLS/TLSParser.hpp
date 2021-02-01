#ifndef PROXYTCP_TLSPARSER_HPP
#define PROXYTCP_TLSPARSER_HPP

#include <string>
#include <src/ByteStream/ByteStream.hpp>

namespace Proxy::TrafficParsing
{
    class TLSParser
    {
    public:
        static uint16_t GetTLSRecordPayloadSize(const uint8_t* recordData, std::size_t bufferSize) noexcept;
        static uint32_t GetTLSMessageSize(const uint8_t* messageData, std::size_t bufferSize) noexcept;
        static bool IsClientHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept;
        static bool IsServerHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept;
        static Status GetDomainNameFromTLSPacket(const uint8_t* buffer, uint32_t bufferSize, ByteStream& emptyByteStream) noexcept;
        static uint16_t ExtractUint16FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept;
        static uint32_t ExtractUint32FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept;
        static Status ExtractAllValidRecords(ByteStream& receivedData, ByteStream& validRecords) noexcept;
        static Status CopyEveryRecordPayloadToMessageBuffer(const ByteStream& connectionBuffer, ByteStream& messageData) noexcept;
        static Status CopyEveryRecordPayloadToMessageBuffer(const ByteStream& connectionBuffer, ByteStream& messageData, ByteStream& changeCipherSpecRecord, ByteStream& otherRecords, std::size_t& expectedBytesInTheNextPacket) noexcept;
        static Status ProcessEveryFullMessagAndGenerateRecords(ByteStream& messageBuffer, ByteStream& emptyRecordBuffer) noexcept;
    };

}

#endif //PROXYTCP_TLSPARSER_HPP
