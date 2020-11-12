#ifndef PROXYTCP_CONVERSATIONMANAGER_HPP
#define PROXYTCP_CONVERSATIONMANAGER_HPP

#include "src/Utilities/NonCopyable.hpp"
#include "src/Connection/Connection.hpp"
#include "src/DataBuffer/DataBuffer.hpp"
#include <unordered_map>
#include <memory>
#include <list>
#include <src/Utilities/Status.hpp>
#include <vector>

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
        Connection GetClientConnectionByServer(const Connection& serverConnection) const noexcept;

        void AddNewPipeline(const ConversationPipeline& pipeline) noexcept;
        void AddNewPipeline(Connection clientConnection, Connection serverConnection) noexcept;

        static std::vector<char> GenerateClientInitiationResponse() noexcept;
        static std::vector<char> GenerateServerConnectionSuccessResponse(const std::string& destinationAddress, uint16_t port, uint8_t addressType) noexcept;


        static Utilities::Status ReadFromConnection  (std::vector<char>& buffer, const Connection& connection) noexcept;
        static Utilities::Status ReadFixedSizeFromConnection  (std::vector<char>& buffer, const Connection& connection, uint32_t size) noexcept;

        //TODO: send should return status
        static Utilities::Status SendTo    (const std::vector<char>& buffer, const Connection& recipientConnection) noexcept;
//        static void SendToClientByServer(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;
//        static void SendToServerByClient(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;

//        static void SendFromServerToClient(const  DataBuffer& buffer, const Connection& serverConnection, cons) noexcept;
//        static void SendFromClientToServer(const  DataBuffer& buffer, const Connection& recipientConnection) noexcept;

    private:
        std::list<ConversationPipeline> conversationPipelines;
    };
}



#endif //PROXYTCP_CONVERSATIONMANAGER_HPP
