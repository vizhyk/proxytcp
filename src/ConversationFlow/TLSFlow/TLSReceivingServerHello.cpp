#include "TLSReceivingServerHello.hpp"
#include "TLSDefault.hpp"
#include "TrafficParsing/TLS/TLSParser.hpp"
#include "Utilities/Constants.hpp"
#include "Connection/SocketConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp"

namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSReceivingServerHello::PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        Status status;

        status = ReadAllDataFromConnection(serverConnection);
        if(status.Failed()) { return nullptr; }

        auto pipeline =  serverConnection.Pipeline().lock();
        if(!pipeline) { return nullptr; }

        if(!pipeline->PayloadIsInitialized())
        {
            pipeline->InitPayloadAs<TLSPayloadBuffer>();
        }
        auto& payloadHolder = pipeline->PayloadAs<TLSPayloadBuffer>();

        ByteStream recordBuffer;
        ByteStream changeCipherSpecRecordHolder;
        ByteStream otherRecordsHolder;

        if(payloadHolder.Optdata().waitingForData)
        {
            // case when we got data less then expected.
            if(payloadHolder.Optdata().expectedBytesInTheNextPacket > serverConnection.Buffer().GetUsedBytes())
            {
                auto bytesToWait = payloadHolder.Optdata().expectedBytesInTheNextPacket - serverConnection.Buffer().GetUsedBytes();
                payloadHolder.data().Insert(serverConnection.Buffer().GetBuffer(), serverConnection.Buffer().GetUsedBytes());
                payloadHolder.Optdata().expectedBytesInTheNextPacket = bytesToWait;

                serverConnection.Buffer().Clear();
                return nullptr;
            }

            //save expected PART OF DATA to payloadHolder. That means that we're saving part of the message we were waiting for
            payloadHolder.data().Insert(serverConnection.Buffer().GetBuffer(), payloadHolder.Optdata().expectedBytesInTheNextPacket);

            //then we remove the part we've processed, so now we can work with the other records
            serverConnection.Buffer().Erase(serverConnection.Buffer().Begin(),
                                            serverConnection.Buffer().Begin() + payloadHolder.Optdata().expectedBytesInTheNextPacket,
                                            payloadHolder.Optdata().expectedBytesInTheNextPacket);

            //and now we can process new records arrived.
            status = TrafficParsing::TLSParser::CopyEveryRecordPayloadToMessageBuffer(serverConnection.Buffer(),
                                                                                      payloadHolder.data(),
                                                                                      changeCipherSpecRecordHolder,
                                                                                      otherRecordsHolder, payloadHolder.Optdata().expectedBytesInTheNextPacket );

        }
        else
        {
            status = TrafficParsing::TLSParser::CopyEveryRecordPayloadToMessageBuffer(serverConnection.Buffer(),
                                                                                      payloadHolder.data(),
                                                                                      changeCipherSpecRecordHolder,
                                                                                      otherRecordsHolder, payloadHolder.Optdata().expectedBytesInTheNextPacket );
        }

        if(status.Failed()) { return nullptr; }

        status = TrafficParsing::TLSParser::ProcessEveryFullMessagAndGenerateRecords(payloadHolder.data(), recordBuffer);
        if(status == Status::Success::WaitingForData) { return nullptr; }
        if(status == Status::Success::GeneratedRecordsAndWaitForData)
        {
            recordBuffer.Insert(changeCipherSpecRecordHolder);

            status = SendAllDataFromConnection(recordBuffer, serverConnection);
            if(status.Failed()) { return nullptr; }

            payloadHolder.Optdata().waitingForData = true;

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();

            return nullptr;
        }
        if(status == Status::Success::Success)
        {
            if(!changeCipherSpecRecordHolder.IsEmpty())
            {
                // if change cipher spec was met
                recordBuffer.Insert(changeCipherSpecRecordHolder);
            }

            status = SendAllDataFromConnection(recordBuffer, serverConnection);
            if(status.Failed()) { return nullptr; }

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
            payloadHolder.data().Clear();
            payloadHolder.Optdata().waitingForData = false;

            if(!otherRecordsHolder.IsEmpty())
            {
                //all data that should be considered afre handshake is finished is pushed itno the clear buffer
                //so we can process it as Application Data
                payloadHolder.data().Insert(otherRecordsHolder);
            }
            return std::make_unique<TLSDefault>();

        }

        clientConnection.Buffer().Clear();
        serverConnection.Buffer().Clear();
        return nullptr;

    }

}
