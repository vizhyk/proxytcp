#include "ConnectionManager.hpp"

namespace Proxy
{
    Utilities::Status
    ConnectionManager::BindSocketToPort(int32_t _sockfd, uint16_t port) noexcept
    {
        Utilities::Status status {};

        sockaddr_in socketData {};

        _sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(_sockfd == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        int32_t enable {1};
        if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
        {
            status = Utilities::Status(Utilities::Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        status = MakeSocketNonblocking(_sockfd);
        if(status.Failed()) { return status; }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));

        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(_sockfd);

        if(bind(_sockfd, reinterpret_cast<sockaddr*>(&socketData), sizeof(socketData)) == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::BadBindListeningPortToSocket);
            return status;
        }

        if(listen(_sockfd, 5) == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::MarkSocketPassive);
            return status;
        }

        return status;
    }

    Utilities::Status
    ConnectionManager::CreateSocketForForwardingByIP(int32_t& socketForForwarding, int32_t destinationPort, const std::string& addr) noexcept
    {
        Utilities::Status status {};

        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        char hostName[NI_MAXHOST];

        destinationHost = gethostbyaddr(addr.c_str(), addr.size(), AF_INET);
        if(destinationHost == nullptr)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadDestinationHost);
            return status;
        }

        memset(&destinationAddress, 0, sizeof(destinationAddress));

        destinationAddress.sin_family = AF_INET;
        destinationAddress.sin_port = destinationPort;

        memcpy(reinterpret_cast<char*>(&destinationAddress.sin_addr.s_addr), const_cast<char*>(addr.c_str()), addr.size());

        if(getnameinfo(reinterpret_cast<sockaddr*>(&destinationAddress),sizeof(destinationAddress),hostName, sizeof(hostName), nullptr, 0, 0) != 0)
        {
            status = Utilities::Status::Error::BadGetNameInfo;
            return status;
        }

        socketForForwarding = socket(AF_INET, SOCK_STREAM, 0);
        if(socketForForwarding == -1)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadForwardingSocketCreation);
            return status;
        }

        status = MakeSocketNonblocking(socketForForwarding);
        if(status.Failed()) { return status; }

        auto connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));
        if(connectResult == -1)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadConnectionSocketToAddress);
            return status;
        }

        return status;
    }

    Utilities::Status
    ConnectionManager::CreateSocketForForwardingByHostname(int32_t& socketForForwarding, int32_t destinationPort, const std::string& hostname) noexcept
    {
        Utilities::Status status {};

        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        destinationHost = gethostbyname(hostname.c_str());
        if(destinationHost == nullptr)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadDestinationHost);
            return status;
        }

        memset(&destinationAddress, 0, sizeof(destinationAddress));

        destinationAddress.sin_family = AF_INET;
        memcpy(reinterpret_cast<char*>(&destinationAddress.sin_addr.s_addr),reinterpret_cast<char*>(destinationHost->h_addr), destinationHost->h_length);
        destinationAddress.sin_port = htons(destinationPort);

        socketForForwarding = socket(AF_INET, SOCK_STREAM, 0);
        if(socketForForwarding == -1)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadForwardingSocketCreation);
            return status;
        }

        status = MakeSocketNonblocking(socketForForwarding);
        if(status.Failed()) { return status; }

        auto connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));
        if(connectResult == -1)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadConnectionSocketToAddress);
            return status;
        }

        return status;
    }

    Utilities::Status
    ConnectionManager::MakeSocketNonblocking(int32_t socket) noexcept
    {
        Utilities::Status status {};
        auto result = fcntl(socket,F_SETFL, O_NONBLOCK);
        if(result < 0)
        {
            status = Utilities::Status::Error::BadMakingSocketNonblocking;
            return status;
        }
        return status;
    }

    Utilities::Status
    ConnectionManager::AddConnectionPipeline(Connection clientConnection, Connection serverConnection) noexcept
    {
        Utilities::Status status {};

        //TODO: refactor to return status
        conversationManager_.AddNewPipeline(clientConnection,serverConnection);

        return status;
    }

    Connection
    ConnectionManager::GetConnection(int32_t socket) const noexcept
    {
        auto tmpConnection = socketConnectionMapping_.find(socket);
        if( tmpConnection != socketConnectionMapping_.end())
        {
            return tmpConnection->second;
        }
        //TODO: Proccess NoConnectionObjFound error
        std::cout << "No Connection Found\n";

    }

    Connection
    ConnectionManager::GetServerConnectionByClientSocket(int32_t clientSocket) const noexcept
    {
        auto connectionFindResult = socketConnectionMapping_.find(clientSocket);
        auto clientConnection = (connectionFindResult != socketConnectionMapping_.end()) ? connectionFindResult->second : Connection{};
        auto serverConnection = conversationManager_.GetServerConnectionByClient(clientConnection);
        //TODO: Process NoServerConnectionFound Error
//        if(serverConnection == nullptr)
//        {
//
//            std::cout << "NO SERVER CONNECTION FOUND.\n";
//        }
        return serverConnection;
    }

    [[noreturn]] void
    ConnectionManager::ProcessConnections(const ConnectionInfo& info)
    {
        int32_t listeningSocket {};

        Utilities::Status status {};

        status = ConnectionManager::BindSocketToPort(listeningSocket, info.GetListeningPort());
        //TODO: Process Status failed error somehow;
        if(status.Failed()) { std::cout << "status failed\n"; }

        UpdateBiggestSockfd(listeningSocket);

        FD_ZERO(&nonblockingSockets_);
        FD_SET(listeningSocket, &nonblockingSockets_);

        timeval timeout {};
        timeout.tv_sec = 3 * 60;

        epoll_event ev {}, events[10];
        int32_t epollfd {}, nfds {};

        epollfd = epoll_create(0);
        if(epollfd == -1)
        {
            std::cout << "bad epoll\n";
        }

        ev.events = EPOLLIN;
        ev.data.fd = listeningSocket;
        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, listeningSocket, &ev) == -1)
        {
            std::cout << "epolctl: bad socket\n";
        }

        while(true)
        {
            nfds = epoll_wait(epollfd, events, 10, -1);
            if(nfds == -1)
            {
                std::cout << "Bad epollWait\n";
            }

            for(int i = 0; i < nfds; ++i)
            {
                if(events[i].data.fd == listeningSocket)
                {
                    int32_t newConnectionSocket = accept(listeningSocket, nullptr, nullptr);
                    if(newConnectionSocket == -1)
                    {
                        status = Utilities::Status::Error::BadConnectionFromListeningSocket;
                        PrintStatusAndTerminateProcess(status);
                    }

                    status = MakeSocketNonblocking(newConnectionSocket);
                    if(status.Failed()) { PrintStatusAndTerminateProcess(status); }

                    ev.events = EPOLLIN | EPOLLET;
                    ev.data.fd = newConnectionSocket;

                    if(epoll_ctl(epollfd,EPOLL_CTL_ADD, newConnectionSocket, &ev) == -1)
                    {
                        std::cout << "epolctl: bad socket\n";
                    }

                    status = AddNewConnection({events[i].data.fd, Connection::Side::Client});
                    if(status.Failed()) { PrintStatus(status); }
                }

                else
                {
                    auto socketConnection = GetConnection(events[i].data.fd);
                    bool socks5ConnectionIsEstablished = false;
                    if(socks5ConnectionIsEstablished)
                    {

                    }
                    else
                    {
                        if(socketConnection.GetSide() == Connection::Side::Client)
                        {
                            DataBuffer buffer {512};

                            ConversationManager::ReadFromConnection(buffer, socketConnection);

                            if( SOCKSParser::IsClientInitiationMessage(buffer.GetBuffer(),buffer.GetUsedDataSize()) )
                            {
                                std::string serverAddress {};
                                uint16_t serverPort {};

                                auto response = ConversationManager::GenerateClientInitiationResponse();
                                ConversationManager::SendTo(buffer, socketConnection);

                                ConversationManager::ReadFromConnection(buffer, socketConnection);
                                SOCKSParser::GetDestinationAddressAndPort(buffer.GetBuffer(), buffer.GetUsedDataSize(), serverAddress, serverPort);

                                auto addressType = SOCKSParser::GetDestinationAddressType(buffer.GetBuffer(),buffer.GetUsedDataSize());
                                int32_t serverSocket {};

                                status = TryConnectToTheServer(serverSocket, serverAddress, serverPort, addressType);
                                if(status.Failed()) { PrintStatusAndTerminateProcess(status); }
                                //TODO: status of the following 2 operations
                                const Connection serverConnection {serverSocket, Connection::Side::Server};
                                AddNewConnection(serverConnection);
                                AddConnectionPipeline(socketConnection, serverConnection);

                                buffer = ConversationManager::GenerateServerConnectionSuccessResponse(serverAddress, serverPort, addressType);
                                ConversationManager::SendTo(buffer,serverConnection);
                            }

                            ConversationManager::SendTo(buffer,GetServerConnectionByClientSocket(socketConnection.GetSocketfd()));

                        }
                    }
                }
            }
        }


    }

    void
    ConnectionManager::PrintStatus(const Utilities::Status& status) noexcept
    {
        std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
    }

    void
    ConnectionManager::PrintStatusAndTerminateProcess
            (const Utilities::Status& status) noexcept
    {
        std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
        exit(status.Code());
    }

    Utilities::Status ConnectionManager::AddNewConnection(const Connection& connection) noexcept
    {
        //TODO: Handle errors while emplace
        socketConnectionMapping_.emplace(std::make_pair( connection.GetSocketfd(),connection ));
    }

    Utilities::Status ConnectionManager::TryConnectToTheServer(int32_t& serverSocket, const std::string& serverAddress,
                                                               uint16_t serverPort, uint8_t addressType) noexcept
    {
        Utilities::Status status {};

        if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv6) ||
            addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::IPv4))
        {
            status = CreateSocketForForwardingByIP(serverSocket, serverPort,serverAddress);
            if (status.Failed()) { PrintStatus(status); return status;}
        }

        if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::AddressType::DomainName))
        {
            status = CreateSocketForForwardingByHostname(serverSocket, serverPort, serverAddress);
            if (status.Failed()) { PrintStatus(status); return status; }
        }
    }
}
