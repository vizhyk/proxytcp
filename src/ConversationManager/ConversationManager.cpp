#include "ConversationManager.hpp"

#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>

namespace Proxy
{
    void ConversationManager::AddNewPipeline(Connection clientConnection, Connection serverConnection) noexcept
    {
        conversationPipelines.emplace_back(ConversationPipeline{clientConnection,serverConnection});
    }

    void ConversationManager::AddNewPipeline(const ConversationManager::ConversationPipeline& pipeline) noexcept
    {
        conversationPipelines.emplace_back(pipeline);

    }

    Connection
    ConversationManager::GetServerConnectionByClient(const Connection& clientConnection) const noexcept
    {
        for(const auto& it: conversationPipelines)
        {
            if(it.clientConnection == clientConnection)
            {
                return it.serverConnection;
            }
        }

        //TODO: return valid undefined state;

    }

    void ConversationManager::ReadFromConnection(DataBuffer& buffer, const Connection& connection) noexcept
    {
        while(true)
        {
            int32_t recievedData = recv(connection.GetSocketfd(),buffer.GetBuffer(),buffer.GetAvailableBytes(), 0);
            if(recievedData == -1)
            {
                //TODO: print error
                break;
            }
            if(recievedData == 0)
            {
                //TODO: print status
                break;
            }
            buffer.MarkDataAsRead(recievedData);

        }
    }

    void ConversationManager::SendTo(const DataBuffer& buffer, const Connection& recipientConnection) noexcept
    {
        int32_t totalSendedData {};

        while(totalSendedData < buffer.GetUsedDataSize())
        {
            int32_t sendedData = send(recipientConnection.GetSocketfd(), buffer.GetBuffer(), buffer.GetUsedDataSize(), 0);
            if(sendedData == -1)
            {
                //TODO: process error
                break;
            }

            totalSendedData += sendedData;

        }
    }

    DataBuffer ConversationManager::GenerateClientInitiationResponse() noexcept
    {
        DataBuffer dataBuffer{2};
        dataBuffer[0] = 0x5;
        dataBuffer[1] = 0x0;

        return dataBuffer;
    }

    DataBuffer
    ConversationManager::GenerateServerConnectionSuccessResponse(const std::string& destinationAddress, uint16_t port, uint8_t addressType) noexcept
    {
        //TODO: make constants understandable (5 + 2 + 1);

        DataBuffer response {5 + destinationAddress.size() + 2 + 1};
        response[0] = 0x05;
        response[1] = 0x00;
        response[2] = 0x00;
        response[3] = addressType;

        int8_t destinationAddressSize = destinationAddress.size();
        memcpy(response.GetBuffer() + 4, &destinationAddressSize, sizeof(addressType));
        memcpy(response.GetBuffer() + 5, destinationAddress.c_str(), destinationAddress.size());

        auto htonsPort = htons(port);
        memcpy(response.GetBuffer() + 5 + destinationAddress.size(), &htonsPort, sizeof(htonsPort));

        return response;


    }
}


