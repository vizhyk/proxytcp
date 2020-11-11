#ifndef PROXYTCP_CONVERSATIONMANAGER_HPP
#define PROXYTCP_CONVERSATIONMANAGER_HPP

#include "src/Utilities/NonCopyable.hpp"
#include "src/Connection/Connection.hpp"
#include "src/DataBuffer/DataBuffer.hpp"
#include <unordered_map>
#include <memory>
#include <list>

namespace Proxy
{

    class ConversationManager : public NonCopyable
    {
    public:
        struct ConversationPipeline
        {
            Connection clientConnection;
            Connection serverConnection;
        };
    public:
        Connection GetServerConnectionByClient(const Connection& clientConnection) const noexcept;

        void AddNewPipeline(const ConversationPipeline& pipeline) noexcept;
        void AddNewPipeline(Connection clientConnection, Connection serverConnection) noexcept;

        static DataBuffer GenerateClientInitiationResponse() noexcept;
        static DataBuffer GenerateServerConnectionSuccessResponse(const std::string& destinationAddress, uint16_t port, uint8_t addressType) noexcept;


        static void ReadFromConnection  (       DataBuffer& buffer, const Connection& connection) noexcept;

        //TODO: send should return status
        static void SendTo    (const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;
//        static void SendToClientByServer(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;
//        static void SendToServerByClient(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;

//        static void SendFromServerToClient(const  DataBuffer& buffer, const Connection& serverConnection, cons) noexcept;
//        static void SendFromClientToServer(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;

    private:
        std::list<ConversationPipeline> conversationPipelines;
    };
}



#endif //PROXYTCP_CONVERSATIONMANAGER_HPP
