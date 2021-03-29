#include "src/TrafficParsing/TLS/TLSParser.hpp"
#include "src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp"
#include "TLSDefault.hpp"
#include "Connection/SocketConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationFlow/BreakConnection.hpp"
namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSDefault::PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        Status status;

        bool clientSideEvent;

        if (sockfdWithEvent == clientConnection.GetSocketfd())
        {
            clientSideEvent = true;
            status = ReadAllDataFromConnection(clientConnection);
            if(status  == Status::Success::DataTransferEnded)
            {
                return std::make_unique<BreakConnection>();
            }
            if(clientConnection.Buffer().GetUsedBytes() == 0) { return nullptr; }
        }
        else
        {
            clientSideEvent = false;
            status = ReadAllDataFromConnection(serverConnection);
            if(status  == Status::Success::DataTransferEnded)
            {
                return std::make_unique<BreakConnection>();
            }
            if(serverConnection.Buffer().GetUsedBytes() == 0) { return nullptr; }
        }

        if(status.Failed()) { return nullptr; }

        auto pipeline =  clientConnection.Pipeline().lock();
        if(!pipeline) { return nullptr; }


        if(!pipeline->PayloadIsInitialized())
        {
            pipeline->InitPayloadAs<TLSPayloadBuffer>();
        }
        auto& unprocessedRecords = pipeline->PayloadAs<TLSPayloadBuffer>();

        ByteStream validRecords;

        if(clientSideEvent)
            { unprocessedRecords.data().Insert(clientConnection.Buffer()); }
        else
            { unprocessedRecords.data().Insert(serverConnection.Buffer()); }

        status = TrafficParsing::TLSParser::ExtractAllValidRecords(unprocessedRecords.data(), validRecords);
        if(status == Status::Success::Success)
        {
            unprocessedRecords.data().Clear();
        }

        if(!validRecords.IsEmpty())
        {
            if(clientSideEvent)
            {
                status = SendAllDataFromConnection(validRecords, clientConnection);
            }
            else
            {
                status = SendAllDataFromConnection(validRecords, serverConnection);
            }

            if(status.Failed()) { return nullptr; }

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
            return nullptr;
        }

        clientConnection.Buffer().Clear();
        serverConnection.Buffer().Clear();

        return nullptr;
    }

}
