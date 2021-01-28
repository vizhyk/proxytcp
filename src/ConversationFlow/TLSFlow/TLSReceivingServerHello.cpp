#include "TLSReceivingServerHello.hpp"
#include "TLSDefault.hpp"
#include "TrafficParsing/TLS/TLSParser.hpp"
#include "Utilities/Constants.hpp"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "src/ConversationPipeline/ConversationPipeline.hpp"
#include <src/ConversationPipeline/PayloadBuffer/TLSPayloadBuffer.hpp>

namespace Proxy::TLSFlow
{
    std::unique_ptr<ConversationFlow>
    TLSReceivingServerHello::PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {

        Status status;
        status = ReadAllDataFromConnection(serverConnection);
        if(status.Failed()) { return nullptr; }

        if(!serverConnection.Pipeline()->PayloadIsInitialized())
        {
            serverConnection.Pipeline()->InitPayloadAs<TLSPayloadBuffer>();
        }
        auto& unprocessedRecords = serverConnection.Pipeline()->PayloadAs<TLSPayloadBuffer>();

        ByteStream recordBuffer;
        ByteStream changeCipherSpecRecord;
        status = TrafficParsing::TLSParser::FindFullServerHelloRecord(serverConnection.Buffer(), recordBuffer,unprocessedRecords.data());

        if(status.Failed() || status == Status::Success::WaitingForData)
        {
            exit(1);
            return nullptr;
        }

        status = SendAllDataToConnection(recordBuffer, clientConnection);
        if(status.Failed())
        {
//            std::cout << "No data was transfered to server\n";
            return nullptr;
        }
//        std::cout << "[Server]: Sent " << recordBuffer.GetUsedBytes() << " bytes to client.\n";

        clientConnection.Buffer().Clear();
        serverConnection.Buffer().Clear();

        return std::make_unique<TLSDefault>();

//        Status status;
//        std::cout << "[TLS_RECORD::ServerHello]\n";
//
//        status = ReadAllDataFromConnection(serverConnection);
//        if(status.Failed()) { return nullptr; }
//        std::cout << "[Received: " << serverConnection.Buffer().GetUsedBytes() << " bytes from server]\n";
//
//        if(!serverConnection.Pipeline()->PayloadIsInitialized())
//        {
//            serverConnection.Pipeline()->InitPayloadAs<TLSPayloadBuffer>();
//        }
//        auto& tlsPayloadBuffer = serverConnection.Pipeline()->PayloadAs<TLSPayloadBuffer>();
//
//        ByteStream validRecords;
//
//        if(tlsPayloadBuffer.Optdata().waitingForMoreData)
//        {
//            ByteStream incompleteData;
//
//            tlsPayloadBuffer.data().Insert(serverConnection.Buffer());
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
//            status = TrafficParsing::TLSParser::ExtractAllValidRecords(serverConnection.Buffer().GetBuffer(),
//                                                                       serverConnection.Buffer().GetUsedBytes(),
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
//
//        if(!validRecords.IsEmpty())
//        {
//            status = SendAllDataToConnection(serverConnection.Buffer(), clientConnection);
//            if(status.Failed())
//            {
//                std::cout << "No data was transfered to client\n";
//                return nullptr;
//            }
//            clientConnection.Buffer().Clear();
//            serverConnection.Buffer().Clear();
//
//            return std::make_unique<TLSDefault>();
//        }
//
//        clientConnection.Buffer().Clear();
//        serverConnection.Buffer().Clear();

//        return nullptr;

    }

}
