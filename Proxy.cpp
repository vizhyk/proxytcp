
#include "Proxy.hpp"

namespace Proxy
{

    Status CreateSocketOnListeningPort(int32_t &listeningSocket, int32_t listeningPort, sockaddr_in &socketData) noexcept
    {
        Status status {};

        listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(listeningSocket == -1)
        {
            status = Status(Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));

        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(listeningPort);

        if(bind(listeningSocket, reinterpret_cast<sockaddr*>(&socketData), sizeof(socketData)) == -1)
        {
            status = Status(Status::Error::BadBindListeningPortToSocket);
            return status;
        }

        if(listen(listeningSocket, 40) == -1)
        {
            status = Status(Status::Error::MarkSocketPassive);
            return status;
        }

        return status;
    }

    Status TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept
    {
        Status status {};

        const int32_t BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        uint32_t totalBytesWritten {};
        int32_t bytesWritten {};
        int32_t bytesRead {};



        while((bytesRead = read(sourceSocket, buffer, BUFFER_SIZE)) > 0)
        {
            totalBytesWritten = 0;
            while(totalBytesWritten < bytesRead)
            {
                int32_t writingSize = bytesRead - totalBytesWritten;
                bytesWritten = write(destinationSocket, buffer + totalBytesWritten, writingSize );

                if(bytesWritten == - 1)
                {
                    status = Status(Status::Error::BadDataWrittenOnForwarding);
                    return status;
                }

                totalBytesWritten += bytesWritten;
            }
        }

        if(bytesRead == -1)
        {
            status = Status(Status::Error::NoDataReadFromSocket);
        }

        shutdown(sourceSocket, SHUT_RD);
        shutdown(destinationSocket, SHUT_WR);

        close(sourceSocket);
        close(destinationSocket);

        return status;

    }

    void PrintStatusAndExit(const Status &status) noexcept
    {
        std::cout << "[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
        exit(status.Code());
    }

    Status CreateSocketForForwarding(int32_t &socketForForwarding, int32_t destinationPort, const char *hostName) noexcept
    {
        Status status {};

        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        destinationHost = gethostbyname(hostName);

        if(destinationHost == nullptr)
        {
            status = Proxy::Status(Proxy::Status::Error::BadDestinationHost);
            Proxy::PrintStatusAndExit(status);
        }

        memset(&destinationAddress, 0, sizeof(destinationAddress));

        destinationAddress.sin_family = AF_INET;

        memcpy(reinterpret_cast<char*>(&destinationAddress.sin_addr.s_addr),
               reinterpret_cast<char*>(destinationHost->h_addr),
               destinationHost->h_length);

        destinationAddress.sin_port = htons(destinationPort);

        socketForForwarding = socket(AF_INET, SOCK_STREAM, 0);

        if(socketForForwarding == -1)
        {
            status = Proxy::Status(Proxy::Status::Error::BadForwardingSocketCreation);
            Proxy::PrintStatusAndExit(status);
        }

        auto connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));

        if(connectResult == -1)
        {
            status = Proxy::Status(Proxy::Status::Error::BadConnectionSocketToAddress);
            Proxy::PrintStatusAndExit(status);
        }

        return status;
    }

    void ForwardingMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Forwarding mode]\n";

        Proxy::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};

        status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort, socketData);
        if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

        bool waitingForConnection { true };
        while(waitingForConnection)
        {
            int32_t destinationSocket {};
            pid_t parentPID {};

            destinationSocket = accept(listeningSocket, nullptr , nullptr);
            if(destinationSocket == -1)
            {
                status = Proxy::Status(Proxy::Status::Error::BadConnectionFromListeningSocket);
                Proxy::PrintStatusAndExit(status);
            }

            parentPID = fork();
            if(parentPID == -1)
            {
                status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
                Proxy::PrintStatusAndExit(status);
            }

            if(parentPID == 0)
            {
                int32_t forwardingSocket {};
                pid_t childPID {};

                status = Proxy::CreateSocketForForwarding( forwardingSocket, fwd.destinationPort, fwd.hostName.c_str());

                // child process forward traffic to the client,
                // parent process forward traffic to the destination[port]
                childPID = fork();
                if(childPID == -1)
                {
                    status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
                    Proxy::PrintStatusAndExit(status);
                }

                if(childPID == 0)
                {
                    std::cout << "[Transfering data from port " << fwd.listeningPort << " to port " << fwd.destinationPort << "]\n";
                    status = Proxy::TransferData(forwardingSocket, destinationSocket);
                    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
                }
                else
                {
                    std::cout << "[Transfering data from port " << fwd.destinationPort << " to port " << fwd.listeningPort << "]\n";
                    status = Proxy::TransferData(destinationSocket, forwardingSocket);
                    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
                }

                exit(static_cast<int32_t>(Proxy::Status::Success::Success));
            }

            close(destinationSocket);
        }

    }

} // namespace Proxy

namespace Proxy::Tracking
{
    void TrackingMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Tracking mode]\n";

        Proxy::Status status {};
        sockaddr saddr {};

        int32_t socketDataSize = sizeof(saddr);
        int32_t rawSocket;

        char buff[4096];

        // decided to use raw socket, bcs SOCK_STREAM cought just ServerHello messages(in my case)
        rawSocket = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
        if(rawSocket < 0)
        {
            status = Proxy::Status(Proxy::Status::Error::BadListeningSocketInitializaton);
            PrintStatusAndExit(status);
        }

        bool waitingForConnection {true};
        while(waitingForConnection)
        {
            auto recievedPackets = recvfrom(rawSocket , buff , 4096 , 0 , &saddr , (socklen_t*)&socketDataSize);
            if(recievedPackets <0 )
            {
                status = Proxy::Status(Proxy::Status::Error::NoDataReadFromSocket);
                PrintStatusAndExit(status);
            }

            iphdr *IPHeader = reinterpret_cast<iphdr*>(buff + sizeof(ethhdr));
            if(IPHeader->protocol == 6) // check if it's TCP
            {
                if(IsClientHelloMesasge(buff, 0))
                {
                    std::string domain = GetDomainNameFromTCPPacket(buff, 0);
                    std::cout << "You have been connected with: " << domain << "\n";
                }
            }

        }
    }

    bool IsClientHelloMesasge(const char* buff, int32_t offset = 0) noexcept
    {
        // offset is used for compatibility with raw sockets.
        // - raw sockets track data from 2 layer OSI
        // - tcp sockets track data from 4 layer OSI
        //      66 is the beginning of the tls packet.

        // buff[66] - position of the TLS Content Type field. [0x16 - Handshake]/[0x17 - Application Data]
        // buff[71] - position of the Handshake Type [0x01 - ClienHello]/[0x02 - ServerHello]
        return ( static_cast<uint32_t>(buff[66-offset]) == 0x16 ) && ( static_cast<uint32_t>(buff[71-offset]) == 0x01 );
    }

    std::string GetDomainNameFromTCPPacket(const char* buffer, uint32_t offset = 0) noexcept
    {
        auto domainNameSize = static_cast<uint32_t>(buffer[192 - offset]);
        char* domainName = new char[domainNameSize];
        memcpy(domainName,buffer+193-offset,domainNameSize);
        std::string tmp(domainName);
        delete[] domainName;
        return tmp;
    }

} //namespace Proxy::Tracking

namespace Proxy::Ban
{
    void BanMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Ban mode]\n";

        Proxy::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};
        int32_t newConnectionSocket {};

        pid_t parentPID;

        // listening on 8081
        status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort, socketData);
        if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

        bool waitingForConnection { true };
        while(waitingForConnection)
        {
            newConnectionSocket = accept(listeningSocket, nullptr , nullptr);
            if(newConnectionSocket == -1)
            {
                status = Proxy::Status(Proxy::Status::Error::BadConnectionFromListeningSocket);
                Proxy::PrintStatusAndExit(status);
            }

            parentPID = fork();

            if(parentPID == 0)
            {
                pid_t childPID {};
                childPID = fork();
                if(childPID == -1)
                {
                    status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
                    Proxy::PrintStatusAndExit(status);
                }

                if(childPID == 0)
                {
                    std::cout << "[Transfering data from port " << fwd.listeningPort << " to port " << fwd.destinationPort << "]\n";

                    status = Proxy::Ban::TransferDataWithRestriction(newConnectionSocket, fwd.bannedHostName, fwd.listeningPort);
                    if (status.Failed() && (status.Code() != static_cast<int32_t>(Proxy::Status::Error::BannedHostDataTransfer))) { Proxy::PrintStatusAndExit(status); }
                }
            }

            close(newConnectionSocket);
        }

    }

    Status TransferDataWithRestriction(int32_t listeningSocket, const std::string& bannedHostname, int32_t listeningPort) noexcept
    {
        const int32_t BUFFER_SIZE = 4096;
        char  buffer [BUFFER_SIZE];

        Status status {};
        sockaddr saddr {};

        int32_t destinationSocket {};
        int32_t socketDataSize = sizeof(saddr);
        int32_t bytesSended {};
        int32_t bytesRead {};
        uint32_t totalBytesSended {};

        bool connectionIsAllowed = false;

        std::string connectedHostDomainName;

        //recieving ALL data that come to our listenignSocket
        while((bytesRead = recvfrom(listeningSocket , buffer , 4096 , 0 , &saddr , (socklen_t*)&socketDataSize)) > 0)
        {
            if(Tracking::IsClientHelloMesasge(buffer, 66))
            {
                connectedHostDomainName = Tracking::GetDomainNameFromTCPPacket(buffer, 66);
                if( connectedHostDomainName == bannedHostname)
                {
                    std::cout << "Connection refused!\n";
                    status = Status::Success::BannedHostConnectionRefused;
                    return status;
                }

                std::cout << "[ClientHello message from: " << connectedHostDomainName << "]\n";
                // if host is not banned - allow connection
                connectionIsAllowed = true;

                status = CreateSocketForForwarding(destinationSocket, 8080, connectedHostDomainName.c_str());
                if(status.Failed()) { PrintStatusAndExit(status); }
            }

            if(connectionIsAllowed)
            {
                totalBytesSended = 0;
                while(totalBytesSended < bytesRead)
                {
                    int32_t writingSize = bytesRead - totalBytesSended;
                    bytesSended = send(destinationSocket, buffer, writingSize, 0);
                    if(bytesSended == - 1)
                    {
                        status = Status(Status::Error::BadDataWrittenOnForwarding);
                        return status;
                    }

                    totalBytesSended += bytesSended;

                    auto recievedFromServer = recv(destinationSocket, buffer, 4096, 0);
                    if(recievedFromServer == -1)
                    {
                        status = Status(Status::Error::BadRecievingDataFromSocket);
                        return status;
                    }

                    auto sendedFromServer = send(listeningSocket, buffer, recievedFromServer, 0);
                    if(sendedFromServer == -1)
                    {
                        status = Status(Status::Error::BadSendingDataToServer);
                        return status;
                    }

                }
            }
        }

        if(bytesRead == -1)
        {
            status = Status(Status::Error::NoDataReadFromSocket);
        }

        shutdown(listeningSocket, SHUT_RD);

        close(listeningSocket);
        close(destinationSocket);
        return status;
    }

    void PrintRecievedData(const char *buffer, uint32_t size) noexcept
    {
        printf("\n\n=============\n\n");
        for(int32_t i = 0; i < size; ++i)
        {
            if( (i%8 == 0) && (i%16!=0) )
                printf("\t");

            if(i%16 == 0)
                printf("\n");

            printf("%02x ", static_cast<uint8_t>(buffer[i]));
        }
    }

} //namespace Proxy::Ban

