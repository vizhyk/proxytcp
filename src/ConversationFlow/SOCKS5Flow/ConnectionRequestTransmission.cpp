#include "TrafficParsing/SOCKS5/SOCKS5Parser.hpp"
#include "Utilities/SOCKS5.hpp"
#include "ConnectionRequestTransmission.hpp"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
#include <netdb.h>
#include "fcntl.h"
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <sys/epoll.h>

namespace Proxy::SOCKS5Flow
{
    Status
    ConnectionRequestTransmission::MakeSocketNonblocking(int32_t socket) noexcept
    {
        Status status {};
        int32_t flags = fcntl(socket, F_GETFL, 0);
        auto result = fcntl(socket, F_SETFL, flags | O_NONBLOCK);
        if(result < 0)
        {
            status = Status::Error::BadMakingSocketNonblocking;
            return status;
        }
        return status;
    }

    int32_t
    ConnectionRequestTransmission::CreateSocketForForwardingByHostname(Status& status, int32_t destinationPort, const uint8_t* hostname, int32_t epollfd) noexcept
    {
        addrinfo hints {};
        addrinfo* res = nullptr;

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        auto gaiResult = getaddrinfo(reinterpret_cast<const char *>(hostname), "https", &hints, &res);
        if(gaiResult != 0)
        {
//            std::cout << "[ConnectionRequestTransmission]: " << gai_strerror(errno) << " | Might be incorrect domain name. " <<  "\n";
            status =  Status(Status::Error::BadGetAddrInfo);
            return -1;
        }

        auto socketForForwarding = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(socketForForwarding == -1)
        {
            status = Status(Status::Error::BadForwardingSocketCreation);
            return -1;
        }

        status = MakeSocketNonblocking(socketForForwarding);
        if(status.Failed())
        {
            close(socketForForwarding);

            status =  Status(Status::Error::BadMakingSocketNonblocking);
            shutdown(socketForForwarding, SHUT_RDWR);

            return -1;
        }

        epoll_event ev {};

        ev.events = EPOLLIN | EPOLLOUT | EPOLLERR;
        ev.data.fd = socketForForwarding;

        if(epoll_ctl(epollfd, EPOLL_CTL_ADD, socketForForwarding, &ev) == -1)
        {
//            std::cout << "[ConnectionRequestTransmission]: " <<  "[M: " << strerror(errno) << " " << errno << " " << EINVAL << "]\n";

            status = Status(Status::Error::BadEpollCTL);
            shutdown(socketForForwarding, SHUT_RDWR);

            return -1;
        }

        auto connectResult = connect(socketForForwarding, res->ai_addr, res->ai_addrlen);
        if(connectResult == -1 && errno == EINPROGRESS)
        {
            status = Status(Status::Success::WaitingForConnectResponse);
            return socketForForwarding;
        }
        else
        {
//            std::cout << "[ConnectionRequestTransmission]: "  << strerror(errno) << "\n";
            status = Status(Status::Error::BadConnectionSocketToAddress);

            shutdown(socketForForwarding, SHUT_RDWR);

            return -1;
        }
    }

    Status
    ConnectionRequestTransmission::CreateSocketForForwardingByIP(int32_t socketForForwarding, int32_t destinationPort, const uint8_t *addr) noexcept
    {
        Status status {};

        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        char hostName[NI_MAXHOST];

        destinationHost = gethostbyaddr(addr, sizeof(addr), AF_INET);
        if(destinationHost == nullptr)
        {
            status =  Status(Status::Error::BadDestinationHost);
            return status;
        }

        memset(&destinationAddress, 0, sizeof(destinationAddress));

        destinationAddress.sin_family = AF_INET;
        destinationAddress.sin_port = destinationPort;

        memcpy(&destinationAddress.sin_addr.s_addr, addr, sizeof (*addr)); // sizeof(ptr) !!!!!

        if(getnameinfo(reinterpret_cast<sockaddr*>(&destinationAddress),sizeof(destinationAddress),hostName, sizeof(hostName), nullptr, 0, 0) != 0)
        {
            status = Status::Error::BadGetNameInfo;
            return status;
        }

        socketForForwarding = socket(AF_INET, SOCK_STREAM, 0);
        if(socketForForwarding == -1)
        {
            status =  Status(Status::Error::BadForwardingSocketCreation);
            return status;
        }

        status = MakeSocketNonblocking(socketForForwarding);
        if(status.Failed())
        {
            close(socketForForwarding);
            return status;
        }

        auto connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));

        if(connectResult == -1)
        {
            status =  Status(Status::Error::BadConnectionSocketToAddress);
            close(socketForForwarding);
            return status;
        }

        return status;
    }

    Status
    ConnectionRequestTransmission::TryConnectToTheServer(ClientConnection& clientConnection, const uint8_t* serverAddress, uint16_t serverPort, uint8_t addressType, int32_t epollfd, int32_t& serverSockfd) noexcept
    {
        using namespace Proxy::Utilities;
        Status status {};

        if (addressType == static_cast<uint8_t>(SOCKS5::Handshake::IPv4) || addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv6))
        {
//            status = CreateSocketForForwardingByIP(serverConnection, serverPort, serverAddress);
            if (status.Failed()) {return status;}

            return status;
        }

        if (addressType == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::DomainName))
        {
            serverSockfd = CreateSocketForForwardingByHostname(status, serverPort, serverAddress, epollfd);
            if(serverSockfd == -1)
            {
                // here might be additional logic that process different errors;
                return status;
            }
            return status;
        }

        status = Status(Status::Error::BadConnectToServer);
        return status;
    }

    Status
    ConnectionRequestTransmission::GenerateConnectionRequestReply(const uint8_t* buffer, size_t buffersize, ByteStream& connectionRequestReply) noexcept
    {
        using namespace Utilities;
        using namespace TrafficParsing;

        Status status;

        auto connectionRequestLength = SOCKS5Parser::GetConnectionRequestLength(buffer, buffersize);

        if( connectionRequestLength != -1 )
        {
            connectionRequestReply.Insert(buffer, buffersize);
            connectionRequestReply[1] = static_cast<uint8_t>(SOCKS5::Handshake::ReplyStatus::Succeeded);
            connectionRequestReply[2] = static_cast<uint8_t>(0x00); // RSV(reserved)
            connectionRequestReply[3] = static_cast<uint8_t>(SOCKS5::Handshake::ATYP::domain);

            return status;
        }

        status = Status(Status::Error::BadConnectionRequestLenght);
        return status;
    }

    std::unique_ptr<ConversationFlow>
    ConnectionRequestTransmission::PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
        using namespace  TrafficParsing;

        Status status;

        if(m_connetionState == ConnectionState::NotConnected)
        {
//            std::cout << "SOCKS5Flow::ConnectionRequest\n";

            ByteStream destinationAddress;

            int32_t serverSockfd = 0;
            uint16_t  port = 0;

            status = ReadAllDataFromConnection(clientConnection);
            if(status.Failed()) { return nullptr; }

            if(SOCKS5Parser::IsValidConnectionRequestMessage(clientConnection.Buffer().GetBuffer(), clientConnection.Buffer().GetUsedBytes()))
            {
                uint8_t destinationAddressType = SOCKS5Parser::GetDestinationAddressType(clientConnection.Buffer().GetBuffer(), clientConnection.Buffer().GetUsedBytes());
                if(destinationAddressType == 0)
                {
                    status = Status(Status::Error::BadDestinationAddressType);
                    return nullptr;
                }

                status = SOCKS5Parser::GetDestinationAddressAndPort(clientConnection.Buffer().GetBuffer(),clientConnection.Buffer().GetUsedBytes(),destinationAddress, port);
                if(status.Failed()) { return nullptr; }

                status = TryConnectToTheServer(clientConnection, destinationAddress.GetBuffer(), port, destinationAddressType, epollfd, serverSockfd);
                if(status.Failed()) { return nullptr; }
                if(status == Status::Success::WaitingForConnectResponse)
                {
                    clientConnection.Pipeline()->InitServerConnection(serverSockfd);
                    clientConnection.Pipeline()->PipelineManager().LinkSockfdToExistingPipeline(serverSockfd,clientConnection.Pipeline());

                    m_connetionState = ConnectionState::WaitingForResponse;
                    return nullptr;
                }
            }
        }

        if(m_connetionState == ConnectionState::WaitingForResponse && IsConnectionSucceed(serverConnection.GetSocketfd()))
        {
            ByteStream connectionRequestReply;

            status = GenerateConnectionRequestReply(clientConnection.Buffer().GetBuffer(), clientConnection.Buffer().GetUsedBytes(), connectionRequestReply);
            if(status.Failed()) { return nullptr; }

            status = SendAllDataToConnection(connectionRequestReply, clientConnection);
            if(status.Failed()) { return nullptr; }

            serverConnection.Buffer().Clear();
            clientConnection.Buffer().Clear();
//            std::cout << "[SOCKS5 Handshake Succeed]\n";
            return std::make_unique<TLSFlow::TLSReceivingClientHello>();
        }

        return nullptr;
    }


    Status
    ConnectionRequestTransmission::IsConnectionSucceed(int32_t sockfd) noexcept
    {
        Status status;

        auto optval = 0;
        socklen_t optlen = sizeof(optval);

        auto opts = getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen);
        if(opts == -1)
        {
            status = Status(Status::Error::BadGetSockopt);
            return status;
        }

        if((optval == EWOULDBLOCK) || (optval == EAGAIN))
        {
            // prefer to close connection and continue program execution
            status = Status(Status::Error::BadConnectToServer);
            return status;
        }

        return status;
    }

}
