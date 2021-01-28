#include <iostream>
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

        if(sockfdWithEvent == clientConnection.GetSocketfd())
        {
            status = ReadAllDataFromConnection(clientConnection);
            if(status.Failed()) { return nullptr; }

//            std::cout << "[Received: " << clientConnection.Buffer().GetUsedBytes() << " bytes from client]\n";
            if(!clientConnection.Pipeline()->PayloadIsInitialized())
            {
                clientConnection.Pipeline()->InitPayloadAs<TLSPayloadBuffer>();
            }
            auto& unprocessedRecords = clientConnection.Pipeline()->PayloadAs<TLSPayloadBuffer>();

            ByteStream validRecords;
            ByteStream incompleteData;

            unprocessedRecords.data().Insert(clientConnection.Buffer());

            status = TrafficParsing::TLSParser::ExtractAllValidRecords(unprocessedRecords.data().GetBuffer(),
                                                                       unprocessedRecords.data().GetUsedBytes(),
                                                                       validRecords,
                                                                       incompleteData);
            if(status == Status::Success::Success)
            {
                unprocessedRecords.Optdata().waitingForMoreData = false;
                unprocessedRecords.data().Clear();
            }

            if(status == Status::Success::WaitingForData)
            {
                unprocessedRecords.data().Clear();
                unprocessedRecords.data().Insert(incompleteData);
            }

            if(!validRecords.IsEmpty())
            {
                status = SendAllDataToConnection(validRecords, serverConnection);
                if(status.Failed())
                {
//                    std::cout << "No data was transfered to server\n";
                    return nullptr;
                }
//                std::cout << "[Sent " << validRecords.GetUsedBytes() << " bytes to server]\n";

                clientConnection.Buffer().Clear();
                serverConnection.Buffer().Clear();
//                std::cout << "[Client]: Sent " << validRecords.GetUsedBytes() << " bytes to server.\n";

//                std::cout << "[Successful move to the next stage]\n";
                return std::make_unique<TLSDefault>();
            }

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();
        }
        else
        {
            status = ReadAllDataFromConnection(serverConnection);
            if(status.Failed()) { return nullptr; }

//            std::cout << "[Received: " << serverConnection.Buffer().GetUsedBytes() << " bytes from server]\n";

            if(!serverConnection.Pipeline()->PayloadIsInitialized())
            {
                serverConnection.Pipeline()->InitPayloadAs<TLSPayloadBuffer>();
            }
            auto& unprocessedRecords = serverConnection.Pipeline()->PayloadAs<TLSPayloadBuffer>();

            ByteStream validRecords;
            ByteStream incompleteData;

            unprocessedRecords.data().Insert(serverConnection.Buffer());

            status = TrafficParsing::TLSParser::ExtractAllValidRecords(unprocessedRecords.data().GetBuffer(),
                                                                       unprocessedRecords.data().GetUsedBytes(),
                                                                       validRecords,
                                                                       incompleteData);
            if(status == Status::Success::Success)
            {
                unprocessedRecords.Optdata().waitingForMoreData = false;
                unprocessedRecords.data().Clear();
            }

            if(status == Status::Success::WaitingForData)
            {
                unprocessedRecords.data().Clear();
                unprocessedRecords.data().Insert(incompleteData);
            }

            if(!validRecords.IsEmpty())
            {
                status = SendAllDataToConnection(validRecords, clientConnection);
                if(status.Failed())
                {
                    return nullptr;
                }
//                std::cout << "[Sent " << validRecords.GetUsedBytes() << " bytes to server]\n";

                clientConnection.Buffer().Clear();
                serverConnection.Buffer().Clear();
//                std::cout << "[Server]: Sent " << validRecords.GetUsedBytes() << " bytes to client.\n";

//                std::cout << "[Successful move to the next stage]\n";
                return std::make_unique<TLSDefault>();
            }

            clientConnection.Buffer().Clear();
            serverConnection.Buffer().Clear();

        }
        return nullptr;
    }

}
