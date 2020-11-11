#ifndef PROXYTCP_CONNECTIONMANAGER_HPP
#define PROXYTCP_CONNECTIONMANAGER_HPP

#include "src/Connection/Connection.hpp"
#include "src/ConnectionInfo/ConnectionInfo.hpp"
#include "src/ConversationManager/ConversationManager.hpp"
#include "src/Utilities/NonCopyable.hpp"
#include "src/Utilities/Status.hpp"
#include "src/NetworkTrafficParser/SOCKSParser.h"


#include <iostream>
#include <cstdint>
#include <cstring>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/epoll.h>

#include <map>
#include <memory>
#include <thread>


namespace Proxy
{
    class ConnectionManager : NonCopyable
    {
    public:
        ~ConnectionManager() noexcept = default;
    public:
        static Utilities::Status BindSocketToPort(int32_t _sockfd, uint16_t port) noexcept;
        static Utilities::Status CreateSocketForForwardingByIP(int32_t& socketForForwarding, int32_t destinationPort, const std::string& addr) noexcept;
        static Utilities::Status CreateSocketForForwardingByHostname(int32_t& socketForForwarding, int32_t destinationPort, const std::string& hostname) noexcept;
        static Utilities::Status MakeSocketNonblocking(int32_t socket) noexcept;


        Utilities::Status
        TryConnectToTheServer(int32_t& serverSocket, const std::string& serverAddress, uint16_t serverPort,
                              uint8_t addressType) noexcept;
        Utilities::Status AddConnectionPipeline(Connection clientConnection,Connection serverConnection) noexcept;
        Utilities::Status AddNewConnection(const Connection& connection) noexcept;
//        Utilities::Status AddConnection(Connection clientConnection) noexcept;
        Connection GetConnection(int32_t socket) const noexcept;
        Connection GetServerConnectionByClientSocket(int32_t clientSocket) const noexcept;

        void UpdateBiggestSockfd(int32_t sockfd) noexcept { biggestSockfd_ = sockfd; };

        [[noreturn]] void ProcessConnections(const ConnectionInfo& info);

        static void PrintStatus(const Utilities::Status& status) noexcept;
        static void PrintStatusAndTerminateProcess(const Utilities::Status& status) noexcept;


    private:
        std::map<int32_t, Connection> socketConnectionMapping_;
        ConversationManager conversationManager_;
        fd_set nonblockingSockets_;
        int32_t biggestSockfd_;

    };
}



#endif //PROXYTCP_CONNECTIONMANAGER_HPP
