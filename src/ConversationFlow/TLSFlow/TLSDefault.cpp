#include <src/TrafficParsing/TLS/TLSParser.hpp>
#include <src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp>
#include "TLSDefault.hpp"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"

namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSDefault::PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        Status status;

        bool clientSideEvent;

        if (sockfdWithEvent == clientConnection.GetSocketfd())
        {
            clientSideEvent = true;
            status = ReadAllDataFromConnection(clientConnection);
            if(clientConnection.Buffer().GetUsedBytes() == 0) { return nullptr; }
        }
        else
        {
            clientSideEvent = false;
            status = ReadAllDataFromConnection(serverConnection);
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
                status = SendAllDataToConnection(validRecords, serverConnection);
            }
            else
            {
                status = SendAllDataToConnection(validRecords, clientConnection);
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
