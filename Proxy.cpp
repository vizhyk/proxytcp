#include <netinet/ip.h>
#include "Proxy.hpp"

namespace Proxy
{

    Status CreateSocketOnListeningPort(int32_t &listeningSocket, int32_t listeningPort, sockaddr_in &socketData) noexcept
    {
        Status status {};

        listeningSocket = socket(AF_INET, SOCK_STREAM, 0);
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
        std::cout << "[Status code: " << status.Code() << "]\n";
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
            status = Proxy::Status(Proxy::Status::Error::BadBindSocketToAddress);
            Proxy::PrintStatusAndExit(status);
        }

        return status;
    }



    void StartForwardingMode(const ForwardingData &fwd) noexcept
    {
        Proxy::Status status {};
        sockaddr_in socketData {};
        std::cout << "[Forwarding mode]\n";

        int32_t listeningSocket {};

        status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort, socketData);
        if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

        bool waitingForConnection { true };
        while(waitingForConnection)
        {
            int32_t clientSocket {};
            pid_t parentPID {};

            clientSocket = accept(listeningSocket, nullptr , nullptr);
            if(clientSocket == -1)
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
                    status = Proxy::TransferData(forwardingSocket, clientSocket);
                    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
                }
                else
                {
                    std::cout << "[Transfering data from port " << fwd.destinationPort << " to port " << fwd.listeningPort << "]\n";
                    status = Proxy::TransferData(clientSocket, forwardingSocket);
                    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
                }

                exit(static_cast<int32_t>(Proxy::Status::Success::Success));
            }

            close(clientSocket);
        }

    }
}

namespace Proxy::Tracking
{
    void StartTrackingMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Tracking mode]\n";

        Proxy::Status status {};
        sockaddr saddr {};
        auto socketDataSize = sizeof(saddr);


        int32_t rawSocket = socket( AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
        if(rawSocket < 0)
        {
            status = Proxy::Status(Proxy::Status::Error::BadListeningSocketInitializaton);
            PrintStatusAndExit(status);
        }

        char buff[4096];

        bool waitingForConnection {true};
        while(waitingForConnection)
        {
            auto recievedPackets = recvfrom(rawSocket , buff , 4096 , 0 , &saddr , (socklen_t*)&socketDataSize);
            if(recievedPackets <0 )
            {
                status = Proxy::Status(Proxy::Status::Error::NoDataReadFromSocket);
                PrintStatusAndExit(status);
            }

            iphdr *IPHearder = reinterpret_cast<iphdr*>(buff + sizeof(ethhdr));
            if(IPHearder->protocol == 6) // check if it's TCP
            {
                if(IsClientHelloMesasge(buff))
                {
                    std::string domain = GetDomainNameFromTCPPacket(buff);
                    std::cout << "You have been connected with: " << domain << "\n";
                }
            }

        }
    }

    bool IsClientHelloMesasge(const char *buff) noexcept
    {
        // buff[66] - position of the TLS Content Type field. [0x16 - Handshake]/[0x17 - Application Data]
        // buff[71] - position of the Handshake Type [0x01 - ClienHello]/[0x02 - ServerHello]
        return (unsigned int)buff[66] == 0x16 && (unsigned int)buff[71] == 0x01;
    }

    std::string GetDomainNameFromTCPPacket(const char *buffer) noexcept
    {
        auto domainNameSize = (uint32_t)buffer[192];
        char* domainName = new char[domainNameSize];
        memcpy(domainName,buffer+193,domainNameSize);
        std::string tmp(domainName);
        delete[] domainName;
        return tmp;
    }
}

namespace Proxy::Ban
{
    void StartBanMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Ban mode]\n";
    }
}
