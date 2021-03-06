#include <iostream>
#include "TLSReceivingClientHello.hpp"
#include "TLSReceivingServerHello.hpp"
#include "Connection/SocketConnection.hpp"
#include "src/TrafficParsing/TLS/TLSParser.hpp"
#include "src/ConversationPipeline/ConversationPipeline.hpp"
#include "src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp"
#include "Utilities/Constants.hpp"

namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSReceivingClientHello::PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        Status status;
        status = ReadAllDataFromConnection(clientConnection);
        if(status.Failed()) { return nullptr; }

        auto pipeline =  clientConnection.Pipeline().lock();
        if(!pipeline) { return nullptr; }

        if(!pipeline->PayloadIsInitialized())
        {
            pipeline->InitPayloadAs<TLSPayloadBuffer>();
        }
        auto& payloadHolder = pipeline->PayloadAs<TLSPayloadBuffer>();

        ByteStream recordBuffer;

        if(payloadHolder.Optdata().waitingForData)
        {
            payloadHolder.data().Insert(serverConnection.Buffer());
        }
        else
        {
            status = TrafficParsing::TLSParser::CopyEveryRecordPayloadToMessageBuffer(clientConnection.Buffer(), payloadHolder.data());
        }

        if(status.Failed()) { return nullptr; }
        if(status == Status::Success::WaitingForData)
        {
            payloadHolder.Optdata().waitingForData = true;
            return nullptr;
        }

        status = TrafficParsing::TLSParser::ProcessEveryFullMessagAndGenerateRecords(payloadHolder.data(), recordBuffer);

        if(status == Status::Success::WaitingForData) { return nullptr; }
        if(status == Status::Success::GeneratedRecordsAndWaitForData)
        {
            if(TrafficParsing::TLSParser::IsClientHelloMessage(recordBuffer.GetBuffer(),recordBuffer.GetUsedBytes(),Utilities::Offsets::TLS::TLS_DATA))
            {
                status = SendAllDataFromConnection(recordBuffer, clientConnection);
                if(status.Failed()) { return nullptr; }

                payloadHolder.Optdata().waitingForData = true;
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
                auto localstatus = TrafficParsing::TLSParser::GetDomainNameFromTLSPacket(recordBuffer.GetBuffer(), recordBuffer.GetUsedBytes(), domainName);
                if(localstatus.Failed())
                {
                    return nullptr;
                }
                std::cout << reinterpret_cast<const char*>(domainName.GetBuffer()) << "\n";

                status = SendAllDataFromConnection(recordBuffer, clientConnection);
                if(status.Failed()) { return nullptr; }

                clientConnection.Buffer().Clear();
                serverConnection.Buffer().Clear();
                payloadHolder.data().Clear();

                return std::make_unique<TLSReceivingServerHello>();
            }

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
            payloadHolder.data().Clear();

            return nullptr;
        }

        clientConnection.Buffer().Clear();
        serverConnection.Buffer().Clear();
        return nullptr;
    }

}