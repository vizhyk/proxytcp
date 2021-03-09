#include "ConversationFlow.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
#include "Connection/SocketConnection.hpp"

namespace Proxy
{
    Status
    ConversationFlow::ReadAllDataFromConnection(SocketConnection& connection) noexcept
    {
        return dynamic_cast<SocketCapturingConnection*>(&connection)->ReadData();
    }

    Status
    ConversationFlow::SendAllDataFromConnection(const ByteStream& data, SocketConnection& connection) noexcept
    {
        return connection.SendData(data);
    }

    Status ConversationFlow::SendAllDataToConnection(const ByteStream& data, SocketConnection& recipient) noexcept
    {
//        Status status;
//        auto onetimeDataSent = send(destination.GetSocketfd(), data.GetBuffer(), data.GetUsedBytes(),
//                                    MSG_NOSIGNAL);
//        if (onetimeDataSent == -1)
//        {
//            status = Status(Status::Error::BadSendingData);
//            return status;
//        }
//
//        return status;

        return SocketCapturingConnection::SendDataTo(data, recipient);



    }

}

