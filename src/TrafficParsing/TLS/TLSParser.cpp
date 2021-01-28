#include "TLSParser.hpp"
#include "src/Utilities/Constants.hpp"
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

    Status TLSParser::GetDomainNameFromTLSPacket(const uint8_t* buffer, uint32_t bufferSize, uint32_t offset, ByteStream& emptyByteStream) noexcept
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

    uint16_t TLSParser::GetTLSRecordPayloadSize(const uint8_t* buffer, std::size_t bufferSize) noexcept
    {
        uint16_t recordSize;
        if(bufferSize < 5)
        {
            return 0;
        }

        memcpy(&recordSize, buffer + 3, sizeof(recordSize));

        return ntohs(recordSize);

    }

    uint32_t TLSParser::GetTLSMessageSize(const uint8_t* buffer, std::size_t bufferSize) noexcept
    {
        uint32_t messageSize = 0;

        if(bufferSize < 3) { return 0; }

        memcpy(&messageSize, buffer, 3);
        messageSize = ntohl(messageSize);
        messageSize = messageSize >> 8;

        return messageSize;
    }

    Status TLSParser::TLSPacketArrivedFully(const uint8_t* buffer, std::size_t bufferSize) noexcept
    {
        Status status;

        std::size_t TLSRecordRequiredBytes;
        std::size_t processedBytes = 0;
        // adding 2 to get TLS_RECORD Message type and size of the first message
        if(bufferSize < HeaderSize::TLS_RECORD + 2) { return status = Status(Status::Error::PacketIsNotFull); }
        if(bufferSize >= HeaderSize::TLS_RECORD + 2)
        {
            while(processedBytes != bufferSize)
            {
                TLSRecordRequiredBytes = 0;

                auto TLSRecordSize = GetTLSRecordPayloadSize(buffer + processedBytes, bufferSize - processedBytes);
                if(TLSRecordSize == 0) { return status = Status(Status::Error::BadBufferSize); }

                TLSRecordRequiredBytes += TLSRecordSize + HeaderSize::TLS_RECORD;
                processedBytes += HeaderSize::TLS_RECORD;

                if(bufferSize == TLSRecordRequiredBytes)
                {
                    auto firstTLSMessageSize = GetTLSMessageSize(buffer + processedBytes + 1, bufferSize - processedBytes);

                    processedBytes += firstTLSMessageSize + HeaderSize::TLS_MESSAGE;

                    if(firstTLSMessageSize == TLSRecordSize - HeaderSize::TLS_MESSAGE) { return status = Status(Status::Success::Success); }
                    if(firstTLSMessageSize <  TLSRecordSize - HeaderSize::TLS_MESSAGE)
                    {
                        while(processedBytes < TLSRecordRequiredBytes)
                        {
                            auto newTLSMessageSize = GetTLSMessageSize(buffer + processedBytes + 1, bufferSize - processedBytes);

                            processedBytes += newTLSMessageSize + HeaderSize::TLS_MESSAGE;
                        }

                        if(processedBytes == TLSRecordRequiredBytes + HeaderSize::TLS_RECORD) { return status = Status(Status::Success::Success); }
                        if(processedBytes >  TLSRecordRequiredBytes + HeaderSize::TLS_RECORD) { return status = Status(Status::Success::WaitingForTLSMessages); }
                    }
                    if(firstTLSMessageSize >  TLSRecordSize - HeaderSize::TLS_MESSAGE)
                    {
                        return status = Status(Status::Success::WaitingForTLSMessages);
                    }
                }
                if(bufferSize <  TLSRecordRequiredBytes) { return status = Status(Status::Error::PacketIsNotFull); }
                if(bufferSize >  TLSRecordRequiredBytes)
                {
                    auto firstTLSMessageSize = GetTLSMessageSize(buffer + processedBytes + 1, bufferSize - processedBytes);
                    if(firstTLSMessageSize == 0) { return status = Status(Status::Error::BadBufferSize); }

                    processedBytes += firstTLSMessageSize + HeaderSize::TLS_MESSAGE;

                    if(firstTLSMessageSize == TLSRecordSize - HeaderSize::TLS_MESSAGE)
                    {
//                    case when expecting 1 more TLS_RECORD record!!!!  use continue; here to move to the new record
                        continue;
                    }
                    if(firstTLSMessageSize <  TLSRecordSize - HeaderSize::TLS_MESSAGE)
                    {
                        uint32_t newTLSMessageSize = 0;
                        while(processedBytes < TLSRecordRequiredBytes)
                        {
                            newTLSMessageSize = GetTLSMessageSize(buffer + processedBytes, bufferSize - processedBytes);

                            processedBytes += newTLSMessageSize + HeaderSize::TLS_MESSAGE;
                        }

                        if(processedBytes == TLSRecordRequiredBytes + HeaderSize::TLS_RECORD) { continue; }
                        if(processedBytes > TLSRecordRequiredBytes)
                        {
                            if(processedBytes < bufferSize) { continue; }
                            if(processedBytes > bufferSize) { return status = Status(Status::Success::WaitingForTLSMessages); }
                        }
                    }
                    if(firstTLSMessageSize >  TLSRecordSize - HeaderSize::TLS_MESSAGE)
                    {
                        TLSRecordRequiredBytes += firstTLSMessageSize - (TLSRecordSize + HeaderSize::TLS_MESSAGE);

                        // in that case we expect more than 1 record in packet.
                        if(bufferSize > TLSRecordRequiredBytes)
                            continue;

                    }
                }
            }

        }

        return status;


    }

    Status TLSParser::ExtractAllValidRecords(const uint8_t* buffer, uint32_t bufferSize, ByteStream& validRecords, ByteStream& incompleteTLSRecords) noexcept
    {
        Status status;

        std::size_t processedBytes = 0;

        while(processedBytes < bufferSize)
        {
            ///////
            auto recordPayloadSize = GetTLSRecordPayloadSize(buffer + processedBytes, bufferSize - processedBytes);
            if(recordPayloadSize == 0) { return status = Status(Status::Error::BadBufferSize); }

            auto fullTLSRecordSize = recordPayloadSize + HeaderSize::TLS_RECORD;

            auto unprocessedBytes = bufferSize - processedBytes;
            if(fullTLSRecordSize <= unprocessedBytes)
            {
                status = TLSRecordParser::IsRecordFull(buffer + processedBytes, fullTLSRecordSize);
                if(status == Status::Success::Success)
                {
                    validRecords.Insert(buffer + processedBytes, fullTLSRecordSize);
                    processedBytes += HeaderSize::TLS_RECORD + recordPayloadSize;
                }
                if(status == Status::Success::WaitingForData)
                {
                    incompleteTLSRecords.Insert(buffer + processedBytes, fullTLSRecordSize);
                    continue;
                }
            }
            else
            {
                status = Status::Success::WaitingForData;
                incompleteTLSRecords.Insert(buffer + processedBytes, unprocessedBytes);
                return status;
            }

        }
        return  status;
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

    Status TLSParser::FindFullServerHelloRecord(const ByteStream& connectionBuffer, ByteStream& recordBuffer, ByteStream& unprocessedRecords) noexcept
    {
        using namespace TrafficParsing;
        Status status;

        uint32_t processedData = 0;
        uint32_t requiredServerHelloPayloadSize = 0;

        const auto* receivedPacketData = connectionBuffer.GetBuffer();
        const auto  receivedPacketSize = connectionBuffer.GetUsedBytes();

        auto firstRecordPayloadSize = GetTLSRecordPayloadSize(receivedPacketData, receivedPacketSize);

        if(firstRecordPayloadSize < receivedPacketSize)
        {
            if(TLSRecordParser::IsRecordFull(receivedPacketData, firstRecordPayloadSize + HeaderSize::TLS_RECORD))
            {
                if(IsServerHelloMessage(receivedPacketData, firstRecordPayloadSize + HeaderSize::TLS_RECORD, Offsets::TLS::TLS_DATA))
                {
                    recordBuffer.Insert(receivedPacketData, firstRecordPayloadSize + HeaderSize::TLS_RECORD);
                    auto restPacketSize = receivedPacketSize - firstRecordPayloadSize - HeaderSize::TLS_RECORD;
                    unprocessedRecords.Insert(receivedPacketData + firstRecordPayloadSize + HeaderSize::TLS_RECORD,restPacketSize);
                    return status;
                }
            }
        }


        return status = Status::Success::WaitingForData;
    }

    Status TLSParser::CopyEveryFulLRecordPayloadToMessageBuffer(const ByteStream& recordBuffer, ByteStream& messageBuffer) noexcept
    {
        uint32_t processedData = 0;
        const auto* recordData = recordBuffer.GetBuffer();
        const auto  totalSizeOfAllFullRecords = recordBuffer.GetUsedBytes();

        while(processedData < totalSizeOfAllFullRecords)
        {
            auto recordPayloadSize = TLSRecordParser::GetTLSRecordPayloadSize(recordData + processedData, totalSizeOfAllFullRecords - processedData);
            processedData += HeaderSize::TLS_RECORD;
            messageBuffer.Insert(recordData + processedData, recordPayloadSize);
            processedData += recordPayloadSize;
        }

        //TODO error-handling
        return Status();
    }

    Status TLSParser::ProcessEveryFullMessagAndGenerateRecords(ByteStream& messageBuffer, ByteStream& emptyRecordBuffer) noexcept
    {
        Status status;
        using namespace Utilities;
        uint32_t processedData = 0;
        const auto* messageData = messageBuffer.GetBuffer();
        const auto  totalSizeOfAllMessages = messageBuffer.GetUsedBytes();
        ByteStream tmpIncompleteMessageHolder;

        while(processedData < totalSizeOfAllMessages)
        {
            auto messagePayloadSize = TLSMessageParser::GetTLSMessageSize(messageData + processedData + Offsets::TLS::MESSAGE_SIZE,
                                                                   totalSizeOfAllMessages - processedData);

            if(messagePayloadSize + HeaderSize::TLS_MESSAGE > totalSizeOfAllMessages - processedData )
            {
                if(emptyRecordBuffer.IsEmpty())
                    return status = Status::Success::WaitingForData;

                tmpIncompleteMessageHolder.Insert(messageData + processedData, totalSizeOfAllMessages - processedData);

                messageBuffer.Clear();
                messageBuffer << tmpIncompleteMessageHolder;

                return status = Status::Success::GeneratedRecordsAndWaitForData;
            }

            emptyRecordBuffer << static_cast<uint8_t>(TLS::ContentType::Handshake);
            emptyRecordBuffer << static_cast<uint16_t>(htons(0x0301)); // TLS version 1.0
            emptyRecordBuffer << static_cast<uint16_t>(htons(messagePayloadSize + HeaderSize::TLS_MESSAGE)); // TLS version 1.0
            emptyRecordBuffer.Insert(messageData + processedData, messagePayloadSize + HeaderSize::TLS_MESSAGE);

            processedData += messagePayloadSize + HeaderSize::TLS_MESSAGE;

        }

        return status = Status::Success::Success;
    }


}


