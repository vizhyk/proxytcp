#include <iostream>
#include "TLSReceivingClientHello.hpp"
#include "TLSReceivingServerHello.hpp"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "src/TrafficParsing/TLS/TLSParser.hpp"
#include "src/ConversationPipeline/ConversationPipeline.hpp"
#include <src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp>
#include "Utilities/Constants.hpp"
#include <vector>

namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSReceivingClientHello::PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        Status status;
        status = ReadAllDataFromConnection(clientConnection);
        if(status.Failed()) { return nullptr; }

        if(!clientConnection.Pipeline()->PayloadIsInitialized())
        {
            clientConnection.Pipeline()->InitPayloadAs<TLSPayloadBuffer>();
        }
        auto& messageBuffer = clientConnection.Pipeline()->PayloadAs<TLSPayloadBuffer>();

        ByteStream recordBuffer;

        status = TrafficParsing::TLSParser::CopyEveryRecordPayloadToMessageBuffer(clientConnection.Buffer(),messageBuffer.data());

        if(status.Failed() || (status == Status::Success::WaitingForData && recordBuffer.IsEmpty()))
        {
            return nullptr;
        }

        status = TrafficParsing::TLSParser::ProcessEveryFullMessagAndGenerateRecords(messageBuffer.data(), recordBuffer);
        if(status == Status::Success::WaitingForData) { return nullptr; }
        if(status == Status::Success::GeneratedRecordsAndWaitForData)
        {
            if(TrafficParsing::TLSParser::IsClientHelloMessage(recordBuffer.GetBuffer(),recordBuffer.GetUsedBytes(),Utilities::Offsets::TLS::TLS_DATA))
            {
                status = SendAllDataToConnection(recordBuffer, serverConnection);
                if(status.Failed())
                {
//                    std::cout << "No data was transfered to server\n";
                    return nullptr;
                }
//                std::cout << "[Client]: Sent " << recordBuffer.GetUsedBytes() << " bytes to server.\n";
//
//                std::cout << "[Client]: Waiting for data.\n";
            }
            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
            return nullptr;

        }
        if(status == Status::Success::Success)
        {
            if(TrafficParsing::TLSParser::IsClientHelloMessage(recordBuffer.GetBuffer(),recordBuffer.GetUsedBytes(),Utilities::Offsets::TLS::TLS_DATA))
            {
                ByteStream domainName;
                auto localstatus = TrafficParsing::TLSParser::GetDomainNameFromTLSPacket(recordBuffer.GetBuffer(),recordBuffer.GetUsedBytes(), Utilities::Offsets::TLS::TLS_DATA, domainName);
                if(localstatus.Failed())
                {
                    return nullptr;
                }
                std::cout << (const char*)domainName.GetBuffer() << "\n";
            }

            status = SendAllDataToConnection(recordBuffer, serverConnection);
            if(status.Failed())
            {
//                std::cout << "No data was transfered to server\n";
                return nullptr;
            }
//            std::cout << "[Client]: Sent " << recordBuffer.GetUsedBytes() << " bytes to server.\n";
            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
            messageBuffer.data().Clear();
            return std::make_unique<TLSReceivingServerHello>();

        }

        clientConnection.Buffer().Clear();
        serverConnection.Buffer().Clear();
        return nullptr;
    }

//        ByteStream validRecords;

//        if(tlsPayloadBuffer.Optdata().waitingForMoreData)
//        {
//            ByteStream incompleteData;
//
//            tlsPayloadBuffer.data().Insert(clientConnection.Buffer());
//
//            status = TrafficParsing::TLSParser::ExtractAllValidRecords(tlsPayloadBuffer.data().GetBuffer(),
//                                                                       tlsPayloadBuffer.data().GetUsedBytes(),
//                                                                       validRecords,
//                                                                       incompleteData);
//            if(status == Status::Success::Success)
//            {
//                tlsPayloadBuffer.Optdata().waitingForMoreData = false;
//                tlsPayloadBuffer.data().Clear();
//            }
//
//            if(status == Status::Success::WaitingForData)
//            {
//                tlsPayloadBuffer.data().Clear();
//                tlsPayloadBuffer.data().Insert(incompleteData);
//            }
//        }
//        else
//        {
//            status = TrafficParsing::TLSParser::ExtractAllValidRecords(clientConnection.Buffer().GetBuffer(),
//                                                                       clientConnection.Buffer().GetUsedBytes(),
//                                                                       validRecords,
//                                                                       tlsPayloadBuffer.data());
//            if (status == Status::Success::Success)
//            {
//                tlsPayloadBuffer.data().Clear();
//            }
//            if(status == Status::Success::WaitingForData)
//            {
//                tlsPayloadBuffer.Optdata().waitingForMoreData = true;
//            }
//        }

//        if(!validRecords.IsEmpty())
//        {
//            if(TrafficParsing::TLSParser::IsClientHelloMessage(validRecords.GetBuffer(),validRecords.GetUsedBytes(),Utilities::Offsets::TLS_RECORD::TLS_DATA))
//            {
//                ByteStream domainName;
//                auto localstatus = TrafficParsing::TLSParser::GetDomainNameFromTLSPacket(validRecords.GetBuffer(),validRecords.GetUsedBytes(), Utilities::Offsets::TLS_RECORD::TLS_DATA, domainName);
//                if(localstatus.Failed())
//                {
//                    return nullptr;
//                }
//                std::cout << (const char*)domainName.GetBuffer() << "\n";
//            }
//
//            status = SendAllDataToConnection(validRecords, serverConnection);
//            if(status.Failed())
//            {
//                std::cout << "No data was transfered to server\n";
//                return nullptr;
//            }
//            clientConnection.Buffer().Clear();
//            serverConnection.Buffer().Clear();
//
//            return std::make_unique<TLSReceivingServerHello>();
//        }


}