#include "ConnectionRequestTransmission.hpp"

namespace Proxy::SOCKS5Flow
{
    using namespace  TrafficParsing;
    Status ConnectionRequestTransmission::MakeSocketNonblocking(int32_t socket) noexcept
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

    Status ConnectionRequestTransmission::CreateSocketForForwardingByHostname(int32_t& socketForForwarding, int32_t destinationPort, const uint8_t* hostname) noexcept
    {
        Status status {};

        sockaddr_in destinationAddress {};
        struct addrinfo hints, *res;
        hostent* destinationHost {};

        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if(getaddrinfo(reinterpret_cast<const char *>(hostname), reinterpret_cast<const char *>(destinationPort), &hints, &res) != 0)
        {
            status =  Status(Status::Error::BadGetAddrInfo);
            return status;
        }

        socketForForwarding = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if(socketForForwarding == -1)
        {
            status = Status(Status::Error::BadForwardingSocketCreation);
            return status;
        }

        status = MakeSocketNonblocking(socketForForwarding);
        if(status.Failed())
        {
            close(socketForForwarding);
            return status;
        }

        auto connectResult = connect(socketForForwarding, res->ai_addr, res->ai_addrlen);
        if(connectResult == -1)
        {
            status = Status(Status::Error::BadConnectionSocketToAddress);
            return status;
        }

        return status;
    }

    Status ConnectionRequestTransmission::CreateSocketForForwardingByIP(int32_t& socketForForwarding, int32_t destinationPort,const uint8_t *addr) noexcept
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

        memcpy(&destinationAddress.sin_addr.s_addr, addr, sizeof (addr));

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

    Status ConnectionRequestTransmission::TryConnectToTheServer(int32_t serverSocket,  const uint8_t *serverAddress, uint16_t serverPort, uint8_t attype, uint8_t serverAddressSize) noexcept
    {
        using namespace Proxy::Utilities;
        Status status {};

        if (attype == static_cast<uint8_t>(SOCKS5::Handshake::IPv4) || attype == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv6))
        {
            status = CreateSocketForForwardingByIP(serverSocket, serverPort,  serverAddress);
            if (status.Failed()) {return status;}
        }

        if (attype == static_cast<uint8_t>(Utilities::SOCKS5::Handshake::DomainName))
        {
            status = CreateSocketForForwardingByHostname(serverSocket, serverPort, serverAddress);
            if (status.Failed()) { return status; }
        }
    }


    int32_t ConnectionRequestTransmission::GenerateConnectionRequestReply(const uint8_t *buffer, size_t buffersize, ByteStream &GeneratedReply) noexcept
    {
        auto buffervariable = SOCKS5Parser::GetConnectionRequestLength(buffer, buffersize);

        if(buffervariable != -1 )
        {
            GeneratedReply.Insert(buffer, buffersize);
            GeneratedReply[1] = static_cast<uint8_t>(Utilities::SOCKS5::Handshake::ReplyStatus::Succeeded);
            GeneratedReply[2] = static_cast<uint8_t>(Utilities::SOCKS5::Handshake::RSV);
            GeneratedReply[3] = static_cast<uint8_t>(Utilities::SOCKS5::Handshake::ATYP::IPv4);
            return 0;
        }
         return -1;
    }


    std::unique_ptr<ConversationFlow>
    ConnectionRequestTransmission::PerformTransaction(Connection& clientConnection, Connection& serverConnection) noexcept
    {

        Status status;
        ByteStream destinationAddress;
        uint16_t  port = 0;

        status = ReadAllDataFromConnection(clientConnection);
        if(status.Failed()) { return nullptr; }

        if(SOCKS5Parser::IsValidConnectionRequestMessage(clientConnection.Buffer().GetBuffer(), clientConnection.Buffer().GetUsedBytes()))
        {
            uint8_t addressType = clientConnection.Buffer()[3];
            status = SOCKS5Parser::GetDestinationAddressAndPort(clientConnection.Buffer().GetBuffer(),clientConnection.Buffer().GetUsedBytes(),destinationAddress, port);
            if(status.Failed()) { return nullptr; }

            status = TryConnectToTheServer(serverConnection.GetSocketfd(),destinationAddress.GetBuffer() , port, addressType , destinationAddress.GetSize());
            if(status.Failed()) { return nullptr; }

            serverConnection.ChangeState(Connection::ConnectionState::Connected);

            ByteStream tmp_client_buffer;

            GenerateConnectionRequestReply(clientConnection.Buffer().GetBuffer(), clientConnection.Buffer().GetSize(),
                                           tmp_client_buffer);

            status = SendAllDataToConnection(tmp_client_buffer,clientConnection);
            if(status.Failed()){ return nullptr; }

            serverConnection.Buffer().Clear();

            return std::make_unique<ConnectionRequestTransmission>(); //Will return TLS FLow

        }
        return nullptr;
    }

}
