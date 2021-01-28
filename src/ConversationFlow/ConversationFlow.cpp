#include <sys/socket.h>
#include <iostream>
#include <signal.h>

#include "ConversationFlow.hpp"

#include "Connection/Connection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
namespace Proxy
{
    Status
    ConversationFlow::ReadAllDataFromConnection(Connection& connection) noexcept
    {
        Status status;
        int32_t receivedData = 0;
        uint8_t tmpBuffer [2048];
        signal(SIGPIPE, SIG_IGN);

        receivedData = recv(connection.GetSocketfd(), tmpBuffer, sizeof(tmpBuffer), 0);

        if(receivedData == -1)
        {
//            connection.Pipeline()->PipelineManager().RemoveUnactivePipeline(connection.GetSocketfd());
            status = Status(Status::Error::BadRecievingDataFromSocket);
            return status;
        }

        if(receivedData == 0)
        {
            status = Status(Status::Success::Success);
            return status;
        }

//        std::cout << "Received: " << receivedData << "\n";
        connection.Buffer().Insert(tmpBuffer, receivedData);

        return status;
    }

    Status
    ConversationFlow::SendAllDataToConnection(const ByteStream& data, Connection& recipientConnection) noexcept
    {
        Status status;
//        signal(MSG_NOSIGNAL, SIG_IGN);
        auto onetimeDataSent = send(recipientConnection.GetSocketfd(), data.GetBuffer() , data.GetUsedBytes(), MSG_NOSIGNAL);
        if(onetimeDataSent == -1)
        {
//            recipientConnection.Pipeline()->PipelineManager().RemoveUnactivePipeline(recipientConnection.GetSocketfd());
            status = Status(Status::Error::BadSendingData);
            return status;
        }

//        std::cout << "Sended: " << onetimeDataSent << "\n";

        return status;
    }

}

