#include "TLSParser.hpp"
#include "src/Utilities/Constants.hpp"
#include "TLSRecordParser.hpp"
#include "TLSMessageParser.hpp"

#include <cstring>
#include <netinet/in.h>
#include <memory>

namespace Proxy::TrafficParsing
{
    using namespace Proxy::Utilities;

    bool TLSParser::IsClientHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept
    {
        if(bufferSize < 5) return false;
        return (static_cast<uint32_t>(buffer[Offsets::TLS::RECORD_TYPE - offset]) == 0x16 ) && (static_cast<uint32_t>(buffer[Offsets::TLS::MESSAGE_TYPE - offset]) == 0x01 );
    }

    bool TLSParser::IsServerHelloMessage(const uint8_t* buffer, uint32_t bufferSize, int32_t offset) noexcept
    {
        if(bufferSize < 5) return false;
        return (static_cast<uint32_t>(buffer[Offsets::TLS::RECORD_TYPE - offset]) == 0x16 ) && (static_cast<uint32_t>(buffer[Offsets::TLS::MESSAGE_TYPE - offset]) == 0x02 );
    }

    Status TLSParser::GetDomainNameFromTLSPacket(const uint8_t* buffer, uint32_t bufferSize, ByteStream& emptyByteStream) noexcept
    {
        Status status;
        uint32_t extensionOffset = 0;
        uint32_t extensionDataSizeOffset = 0;

        extensionDataSizeOffset += Offsets::TLS::SESSION_ID_LENGTH;

        uint8_t sessionIDLength = buffer[Offsets::TLS::SESSION_ID_LENGTH];
        extensionDataSizeOffset += sessionIDLength + sizeof(sessionIDLength);

        uint16_t cipherSuitsLength = ExtractUint16FromNetworkData(buffer + Offsets::TLS::SESSION_ID_LENGTH + sizeof(sessionIDLength) + sessionIDLength, bufferSize, status);
        extensionDataSizeOffset += cipherSuitsLength + sizeof(cipherSuitsLength);

        uint16_t compressionMethodsLengthOffset = Offsets::TLS::SESSION_ID_LENGTH + sessionIDLength + sizeof(sessionIDLength) + cipherSuitsLength + sizeof(cipherSuitsLength);
        uint8_t  compressionMethodsLength = buffer[compressionMethodsLengthOffset];
        extensionDataSizeOffset += compressionMethodsLength + sizeof(compressionMethodsLength);

        uint16_t extensionsDataSize = ExtractUint16FromNetworkData(buffer + extensionDataSizeOffset, bufferSize, status);
        if(status.Failed()) { return status; }

        auto extentionsData = std::make_unique<uint8_t[]>(extensionsDataSize);

        memcpy(extentionsData.get(),buffer + extensionDataSizeOffset + sizeof(extensionsDataSize) , extensionsDataSize);

        while(extensionOffset < extensionsDataSize)
        {
            const uint16_t extentionType = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset, extensionsDataSize - extensionOffset, status);
            if(status.Failed()) { return status; }

            const uint16_t extentionLength = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset + sizeof(extentionType),extensionsDataSize - extensionOffset, status);
            if(status.Failed()) { return status; }

            if(extensionsDataSize - extensionOffset < sizeof(extentionType) + sizeof(extentionLength)) { status = Status::Error::BadBufferSize; return status; }
            extensionOffset+= sizeof(extentionType) + sizeof(extentionLength);

            if(extentionType == 0)
            {
                if(extensionsDataSize - extensionOffset < Offsets::TLS::SNI::SERVER_NAME_LENGTH) { status = Status::Error::BadBufferSize; return status; }
                extensionOffset += Offsets::TLS::SNI::SERVER_NAME_LENGTH;

                const uint16_t domainNameSize = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset, extensionsDataSize - extensionOffset, status);
                if(status.Failed()) { return status; }

                if(extensionsDataSize - extensionOffset < sizeof(domainNameSize)) { status = Status::Error::BadBufferSize; return status; }
                extensionOffset += sizeof(domainNameSize);

                emptyByteStream.Insert(extentionsData.get() + extensionOffset, domainNameSize);

                if(extensionsDataSize - extensionOffset < domainNameSize) { status = Status::Error::BadBufferSize; return status; }
                extensionOffset += domainNameSize;

                break;
            }
            else
            {
                if(extensionsDataSize - extensionOffset < extentionLength) { status = Status::Error::BadBufferSize; return status; }
                extensionOffset += extentionLength;
            }
        }

        return status;

    }

    uint16_t TLSParser::ExtractUint16FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept
    {
        if(bufferSize < 2)
        {
            status = Status(Status::Error::BadBufferSize);
            return 0;
        }

        uint16_t tmpValue;
        memcpy(&tmpValue, buffer , sizeof(uint16_t));
        tmpValue = ntohs(tmpValue);

        status = Status(Status::Success::Success);
        return tmpValue;
    }

    uint32_t TLSParser::ExtractUint32FromNetworkData(const uint8_t* buffer, uint32_t bufferSize, Status& status) noexcept
    {
        if(bufferSize < 4)
        {
            status = Status(Status::Error::BadBufferSize);
            return 0;
        }

        uint32_t tmpValue;
        memcpy(&tmpValue, buffer , sizeof(uint32_t));
        tmpValue = ntohl(tmpValue);

        status = Status(Status::Success::Success);
        return tmpValue;
    }

    uint16_t TLSParser::GetTLSRecordPayloadSize(const uint8_t* recordData, std::size_t bufferSize) noexcept
    {
        uint16_t recordSize;
        if(bufferSize < 5)
        {
            return 0;
        }

        memcpy(&recordSize, recordData + 3, sizeof(recordSize));

        return ntohs(recordSize);

    }

    uint32_t TLSParser::GetTLSMessageSize(const uint8_t* messageData, std::size_t bufferSize) noexcept
    {
        uint32_t messageSize = 0;

        if(bufferSize < 3) { return 0; }

        memcpy(&messageSize, messageData, 3);
        messageSize = ntohl(messageSize);
        messageSize = messageSize >> 8;

        return messageSize;
    }

    Status TLSParser::ExtractAllValidRecords(ByteStream& receivedData, ByteStream& validRecords) noexcept
    {
        Status status;

        std::size_t processedBytes = 0;

        const auto* buffer = receivedData.GetBuffer();

        while(processedBytes < receivedData.GetUsedBytes())
        {
            auto recordPayloadSize = GetTLSRecordPayloadSize(buffer + processedBytes, receivedData.GetUsedBytes() - processedBytes);
            if(recordPayloadSize == 0) { return Status(Status::Error::BadBufferSize); }

            std::size_t fullTLSRecordSize = recordPayloadSize + HeaderSize::TLS_RECORD;

            auto unprocessedBytes = receivedData.GetUsedBytes() - processedBytes;
            if(fullTLSRecordSize <= unprocessedBytes)
            {
                status = TLSRecordParser::IsRecordFull(buffer + processedBytes, fullTLSRecordSize);
                if(status == Status::Success::Success)
                {
                    validRecords.Insert(buffer + processedBytes, fullTLSRecordSize);
                    receivedData.Erase(receivedData.Begin() + processedBytes, receivedData.Begin() + processedBytes + fullTLSRecordSize, fullTLSRecordSize);
                    processedBytes = 0;
                }
                if(status == Status::Success::WaitingForData)
                {
                    status = Status::Success::WaitingForData;
                    return status;
                }
            }
            else
            {
                status = Status::Success::WaitingForData;
                return status;
            }

        }
        return  status;
    }

    Status TLSParser::CopyEveryRecordPayloadToMessageBuffer(const ByteStream& connectionBuffer, ByteStream& messageData, ByteStream& changeCipherSpecRecord, ByteStream& otherRecords, std::size_t& expectedBytesInTheNextPacket) noexcept
    {
        using namespace TrafficParsing;
        Status status;

        uint32_t processedData = 0;
        const auto* receivedPacketData = connectionBuffer.GetBuffer();
        const auto  receivedPacketSize = connectionBuffer.GetUsedBytes();

        while(processedData < receivedPacketSize)
        {
            auto recordContentType = *(receivedPacketData + processedData);
            if(recordContentType == TLS::ContentType::ChangeCipherSpec)
            {
                const uint8_t CHANGE_CHIPERSPEC_MESSAGE_SIZE = 1;
                changeCipherSpecRecord.Insert(receivedPacketData + processedData, HeaderSize::TLS_RECORD + CHANGE_CHIPERSPEC_MESSAGE_SIZE);

                processedData += HeaderSize::TLS_RECORD + CHANGE_CHIPERSPEC_MESSAGE_SIZE;


                otherRecords.Insert(receivedPacketData + processedData, receivedPacketSize - processedData);
                return status;
            }

            auto recordPayloadSize = TLSRecordParser::GetTLSRecordPayloadSize(receivedPacketData + processedData, receivedPacketSize - processedData);
            if(recordPayloadSize > receivedPacketSize - processedData)
            {
                messageData.Insert(receivedPacketData + processedData + HeaderSize::TLS_RECORD,
                                   receivedPacketSize - processedData - HeaderSize::TLS_RECORD);

                // + header size
                expectedBytesInTheNextPacket = recordPayloadSize - (receivedPacketSize - processedData - HeaderSize::TLS_RECORD);
                return Status(Status::Success::WaitingForData);
            }
            else
            {
                messageData.Insert(receivedPacketData + processedData + HeaderSize::TLS_RECORD, recordPayloadSize);
            }

            processedData += recordPayloadSize + HeaderSize::TLS_RECORD;
        }

        return status;
    }

    Status TLSParser::CopyEveryRecordPayloadToMessageBuffer(const ByteStream& connectionBuffer, ByteStream& messageData) noexcept
    {
        using namespace TrafficParsing;
        Status status;

        uint32_t processedData = 0;
        const auto* receivedPacketData = connectionBuffer.GetBuffer();
        const auto  receivedPacketSize = connectionBuffer.GetUsedBytes();

        while(processedData < receivedPacketSize)
        {
            auto recordPayloadSize = TLSRecordParser::GetTLSRecordPayloadSize(receivedPacketData + processedData, receivedPacketSize - processedData);
            if(recordPayloadSize > receivedPacketSize - processedData)
            {
                messageData.Insert(receivedPacketData + processedData + HeaderSize::TLS_RECORD,
                                   receivedPacketSize - processedData - HeaderSize::TLS_RECORD);
            }
            else
            {
                messageData.Insert(receivedPacketData + processedData + HeaderSize::TLS_RECORD, recordPayloadSize);
            }
            processedData += recordPayloadSize + HeaderSize::TLS_RECORD;
        }

        return status;
    }

    Status TLSParser::ProcessEveryFullMessagAndGenerateRecords(ByteStream& messageBuffer, ByteStream& emptyRecordBuffer) noexcept
    {
        using namespace Utilities;
        uint32_t processedData = 0;
        const auto* messageData = messageBuffer.GetBuffer();
        ByteStream tmpIncompleteMessageHolder;

//        while(processedData < messageBuffer.GetUsedBytes())
        while(messageBuffer.GetUsedBytes() != 0)
        {
            auto messagePayloadSize = TLSMessageParser::GetTLSMessageSize(messageData + processedData + Offsets::TLS::MESSAGE_SIZE,
                                                                          messageBuffer.GetUsedBytes() - processedData);

            if(messagePayloadSize + HeaderSize::TLS_MESSAGE > messageBuffer.GetUsedBytes() - processedData )
            {
                if(emptyRecordBuffer.IsEmpty())
                    return Status(Status::Success::WaitingForData);

//                tmpIncompleteMessageHolder.Insert(messageData + processedData, totalSizeOfAllMessages - processedData);
//                messageBuffer.Clear();
//                messageBuffer << tmpIncompleteMessageHolder;

                return Status(Status::Success::GeneratedRecordsAndWaitForData);
            }

            emptyRecordBuffer << static_cast<uint8_t>(TLS::ContentType::Handshake);
            emptyRecordBuffer << static_cast<uint16_t>(htons(0x0303)); // TLS version 1.2
            emptyRecordBuffer << static_cast<uint16_t>(htons(messagePayloadSize + HeaderSize::TLS_MESSAGE));
            emptyRecordBuffer.Insert(messageData + processedData, messagePayloadSize + HeaderSize::TLS_MESSAGE);

            messageBuffer.Erase(messageBuffer.Begin()+processedData,
                                messageBuffer.Begin() + processedData + HeaderSize::TLS_MESSAGE + messagePayloadSize,
                                HeaderSize::TLS_MESSAGE + messagePayloadSize);

//            processedData += messagePayloadSize + HeaderSize::TLS_MESSAGE;

        }

        return Status(Status::Success::Success);
    }


}


