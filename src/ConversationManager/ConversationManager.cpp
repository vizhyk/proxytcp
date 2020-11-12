#include "ConversationManager.hpp"

#include <utility>
#include <sys/socket.h>
#include <netinet/in.h>
#include <src/Utilities/Status.hpp>
#include <vector>
#include <fcntl.h>

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

    Connection
    ConversationManager::GetClientConnectionByServer(const Connection& serverConnection) const noexcept
    {
        for(const auto& it: conversationPipelines)
        {
            if(it.serverConnection == serverConnection)
            {
                return it.clientConnection;
            }
        }

        //TODO: return valid undefined state;
    }

    Utilities::Status ConversationManager::ReadFromConnection(std::vector<char>& buffer, const Connection& connection) noexcept
    {
        Utilities::Status status {};
        int32_t recievedData {};

        while(true)
        {
            if((recievedData = recv(connection.GetSocketfd(),buffer.data() + recievedData,512, 0)) > 0)
            {
                std::cout << "recieved " << recievedData << " bytes.\n";
            }
            if(recievedData == -1)
            {
                int32_t flags = fcntl(connection.GetSocketfd(), F_GETFL, 0);
                std::cout << (flags) << " | " << O_NONBLOCK <<"\n";
                std::cout << (flags & O_NONBLOCK) << "\n";
                status = Utilities::Status(Utilities::Status::Error::BadRecievingDataFromSocket);
                return status;
            }
            if(recievedData == 0)
            {
                status = Utilities::Status(Utilities::Status::Success::Success);
                return status;
            }

        }


        return status;
    }

    Utilities::Status
    ConversationManager::ReadFixedSizeFromConnection(std::vector<char>& buffer, const Connection& connection, uint32_t size) noexcept
    {
        Utilities::Status status {};
        int32_t recievedData {};

        recievedData = recv(connection.GetSocketfd(),buffer.data() + recievedData,512, 0);
        std::cout << "recieved " << recievedData << " bytes.\n";
        if(recievedData == -1)
        {
            int32_t flags = fcntl(connection.GetSocketfd(), F_GETFL, 0);
            std::cout << (flags) << " | " << O_NONBLOCK <<"\n";
            std::cout << (flags & O_NONBLOCK) << "\n";

            status = Utilities::Status(Utilities::Status::Error::BadRecievingDataFromSocket);
            return status;
        }
        if(recievedData == 0)
        {
            status = Utilities::Status(Utilities::Status::Success::Success);
            return status;
        }

        return status;

    }


    Utilities::Status ConversationManager::SendTo(const std::vector<char>& buffer, const Connection& recipientConnection) noexcept
    {
        Utilities::Status status {};
        int32_t totalSendedData {};


        while(totalSendedData < buffer.size())
        {
            int32_t sendedData = send(recipientConnection.GetSocketfd(), buffer.data(), buffer.size(), MSG_DONTWAIT);
            if(sendedData == -1)
            {
                int32_t flags = fcntl(recipientConnection.GetSocketfd(), F_GETFL, 0);
                std::cout << (flags) << " | " << O_NONBLOCK <<"\n";
                std::cout << (flags & O_NONBLOCK) << "\n";

                status = Utilities::Status(Utilities::Status::Error::BadSendingData);
                return status;
            }

            std::cout << "sended " << sendedData << " bytes\n";

            totalSendedData += sendedData;

        }

        return status;
    }

    std::vector<char> ConversationManager::GenerateClientInitiationResponse() noexcept
    {

        std::vector<char>  dataBuffer;
        dataBuffer.reserve(2);
        dataBuffer.push_back(0x5);
        dataBuffer.push_back(0x0);

        return dataBuffer;
    }

    std::vector<char>
    ConversationManager::GenerateServerConnectionSuccessResponse(const std::string& destinationAddress, uint16_t port, uint8_t addressType) noexcept
    {
        //TODO: make constants understandable (5 + 2 + 1);
        std::vector<char> response;
        char r[5 + destinationAddress.size() + 2 + 1];


        response.reserve(5 + destinationAddress.size() + 2 + 1);
        response.push_back(0x05);
        response.push_back(0x00);
        response.push_back(0x00);
        response.push_back(addressType);
        int8_t destinationAddressSize = destinationAddress.size();
        response.push_back(destinationAddressSize);
        for(auto symbol : destinationAddress)
        {
            response.push_back(symbol);
        }
        auto htonsPort = htons(port);
        response.push_back(htonsPort);

//        memcpy(response.data() + 4, &destinationAddressSize, sizeof(addressType));
//        memcpy(response.data() + 5, destinationAddress.c_str(), destinationAddress.size());

        memcpy(response.data() + 5 + destinationAddress.size(), &htonsPort, sizeof(htonsPort));

        return response;


    }



}


