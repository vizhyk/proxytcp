#include "ConversationFlow.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
#include "Connection/SocketConnection.hpp"

namespace Proxy
{
    Status
    ConversationFlow::ReadAllDataFromConnection(SocketConnection& connection) noexcept
    {
        connection.ReadData();
        return connection.ReadData();
    }

    Status
    ConversationFlow::SendAllDataFromConnection(const ByteStream& data, SocketConnection& destination) noexcept
    {
        destination.SendData(data);
        return destination.SendData(data);
    }

    Status ConversationFlow::SendAllDataToConnection(const ByteStream& data, SocketConnection& destination) noexcept
    {
        Status status;
<<<<<<< HEAD
        auto onetimeDataSent = send(destination.GetSocketfd(), data.GetBuffer(), data.GetUsedBytes(),
                                    MSG_NOSIGNAL);
        if (onetimeDataSent == -1)
=======

        auto onetimeDataSent = send(recipientConnection.GetSocketfd(), data.GetBuffer() , data.GetUsedBytes(), MSG_NOSIGNAL);
        if(onetimeDataSent == -1)
>>>>>>> bed8039 (Entry commit)
        {
            status = Status(Status::Error::BadSendingData);
            return status;
        }

        return status;
    }

}

