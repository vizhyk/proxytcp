#include <netinet/ip.h>
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
}

namespace Proxy::Tracking
{
    void StartTrackingMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "[Tracking mode]\n";

        Proxy::Status status {};
        sockaddr saddr {};

        int32_t socketDataSize = sizeof(saddr);
        int32_t rawSocket;

        char buff[4096];

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
        return ( static_cast<uint32_t>(buff[66]) == 0x16 ) && ( static_cast<uint32_t>(buff[71]) == 0x01 );
    }

    std::string GetDomainNameFromTCPPacket(const char *buffer) noexcept
    {
        auto domainNameSize = static_cast<uint32_t>(buffer[192]);
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

        Proxy::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};

        status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort, socketData);
        if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

        bool waitingForConnection { true };
        while(waitingForConnection)
        {
            int32_t destinationSocket {};
            int32_t socketForPacketAnalysis {};
            int32_t socketForForwarding;
            pid_t parentPID {};


            destinationSocket = accept(listeningSocket, nullptr , nullptr);
            if(destinationSocket == -1)
            {
                status = Proxy::Status(Proxy::Status::Error::BadConnectionFromListeningSocket);
                Proxy::PrintStatusAndExit(status);
            }

            parentPID = fork();
            if(parentPID == 0)
            {
                pid_t childPID {};
                status = Proxy::CreateSocketForForwarding(socketForForwarding, fwd.destinationPort,
                                                          fwd.hostName.c_str());

                childPID = fork();
                if(childPID == -1)
                {
                    status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
                    Proxy::PrintStatusAndExit(status);
                }

                if(childPID == 0)
                {
//                    socketForPacketAnalysis = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
//                    if(socketForPacketAnalysis < 0)
//                    {
//                        status = Proxy::Status(Proxy::Status::Error::BadListeningSocketInitializaton);
//                        PrintStatusAndExit(status);
//                    }
                    std::cout << "[Transfering data from port " << fwd.listeningPort << " to port " << fwd.destinationPort << "]\n";
                    status = Proxy::Ban::TransferDataWithRestriction(socketForPacketAnalysis, socketForForwarding, destinationSocket,
                                                                     fwd.bannedHostName,
                                                                     fwd.listeningPort);
                    if(status.Failed() && (status.Code() != static_cast<int32_t>(Proxy::Status::Error::BannedHostDataTransfer)) ) { Proxy::PrintStatusAndExit(status); }


                }
                else
                {
                    if(status.Code() != static_cast<int32_t>(Proxy::Status::Error::BannedHostDataTransfer))
                    {
                        std::cout << "[Transfering data from port " << fwd.destinationPort << " to port " << fwd.listeningPort << "]\n";
                        status = Proxy::TransferData(destinationSocket, socketForForwarding);
                        if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
                    }
                }
            }

//            socketForPacketAnalysis = socket(AF_PACKET , SOCK_RAW , htons(ETH_P_ALL));
//            if(socketForPacketAnalysis < 0)
//            {
//                status = Proxy::Status(Proxy::Status::Error::BadListeningSocketInitializaton);
//                PrintStatusAndExit(status);
//            }

            close(destinationSocket);
            close(socketForPacketAnalysis);

        }
    }

    Status
        TransferDataWithRestriction(int32_t sniffingSocket, int32_t sourceSocket, int32_t destinationSocket,
                                const std::string &bannedHostname, int32_t listeningPort) noexcept
    {
        Status status {};
        sockaddr saddr {};
        const int32_t BUFFER_SIZE = 4096;
        uint32_t totalBytesWritten {};
        int32_t bytesWritten {};
        int32_t bytesRead {};
        int32_t socketDataSize = sizeof(saddr);

        char buffer[BUFFER_SIZE];

        std::string connectedDomainName;
        bool isBanned;



        //recieving ALL data that comes to our sourceSocket(which is SOCK_RAW)
        while((bytesRead = recvfrom(sourceSocket , buffer , 4096 , 0 , &saddr , (socklen_t*)&socketDataSize)) > 0)
        {


//            for(int i = 0; i < bytesRead; ++i)
//            {
//                if(i%16==0)
//                    printf("\n");
//                printf("%02x ", static_cast<uint32_t>(buffer[i]));
//            }
//
//            printf("\n\n===================\n\n");
//            printf("66: %02x, 71: %02x\n", static_cast<uint8_t>(buffer[0]), static_cast<uint8_t>(buffer[5]));

//
//
//            if((ntohs(TCPHearder->th_dport) == listeningPort) && (IPHeader->protocol == 6))
//            {
//                for(int i = 0; i < bytesRead; ++i)
//                {
//                    if(i%16==0)
//                        printf("\n");
//                    printf("%02x ", static_cast<uint32_t>(buffer[i]));
//                }
//
//                printf("\n\n===================\n\n");
//                printf("66: %02x, 71: %02x\n", static_cast<uint8_t>(buffer[52]), static_cast<uint8_t>(buffer[57]));
//
//
//
//            }

            totalBytesWritten = 0;
            while(totalBytesWritten < bytesRead)
            {
                int32_t writingSize = bytesRead - totalBytesWritten;
                bytesWritten = write(destinationSocket, buffer + totalBytesWritten, writingSize);
                if(bytesWritten == - 1)
                {
                    printf("sending someting \n");
                    status = Status(Status::Error::BadDataWrittenOnForwarding);
                    printf("Oh dear, something went wrong with write()! %s | %d \n", strerror(errno) ,errno);
                    return status;
                }
                totalBytesWritten += bytesWritten;
            }


//            This code as I thought would work if I use Raw socket,
//            but in my case if I sniff traffic with the RawSocket - i'm not able to send reply trought SOCK_STREAM
//
//            iphdr *IPHeader = reinterpret_cast<iphdr*>(buffer + sizeof(ethhdr));
//            tcphdr *TCPHearder = reinterpret_cast<tcphdr*>(buffer + sizeof(iphdr) + sizeof(ethhdr));
//            if((ntohs(TCPHearder->th_dport) == listeningPort) && (IPHeader->protocol == 6))
//            {
//                printf("Port = %d\n", ntohs(TCPHearder->th_dport));
//                if(bytesRead > 71)
//                    printf("66: %d, 71: %d",static_cast<uint32_t>(buffer[66]),static_cast<uint32_t>(buffer[71]));
//                if(Tracking::IsClientHelloMesasge(buffer))
//                {
//                    connectedDomainName = Tracking::GetDomainNameFromTCPPacket(buffer);
//                    if (connectedDomainName == bannedHostname)
//                    {
//                        std::cout << "\'" << bannedHostname << "\' is not allowed to connect. Skipping.\n";
//                        isBanned = true;
//                        break;
//                        //                    return status = Status(Status::Error::BannedHostDataTransfer);
//                    }
//                    std::cout << "You have been connected with: " << connectedDomainName << "\n";
//                }
        }




//            iphdr *IPHeader = reinterpret_cast<iphdr*>(buffer + sizeof(ethhdr));
//            auto iphdrlen = sizeof(iphdr);
//            tcphdr *TCPHearder = reinterpret_cast<tcphdr*>(buffer + iphdrlen + sizeof(ethhdr));

//            printf("(ntohs(TCPHearder->th_dport) = %d", ntohs(TCPHearder->th_dport) );
//            printf("((TCPHearder->th_dport) = %d", TCPHearder->th_dport );
//            if((ntohs(TCPHearder->th_dport) == listeningPort) && (IPHeader->protocol == 6))
//            {
//
//                isBanned = false;
//                std::cout << "[Recieved: " << bytesRead << " bytes]\n";
//                // check if that packet's destination port is 8081 (according to the task). IPHeader->protocol == 6 - means that's TCP packet.
//                if(Tracking::IsClientHelloMesasge(buffer))
//                {
//
//                    connectedDomainName = Tracking::GetDomainNameFromTCPPacket(buffer);
//                    if (connectedDomainName == bannedHostname)
//                    {
//                        std::cout << "\'" << bannedHostname << "\' is not allowed to connect. Skipping.\n";
//                        isBanned = true;
//                        break;
//                        //                    return status = Status(Status::Error::BannedHostDataTransfer);
//                    }
//
//                    std::cout << "You have been connected with: " << connectedDomainName << "\n";
//                }
//
//                if(isBanned == false)
//                {
//
//                }
//            }


        if(bytesRead == -1)
        {
            status = Status(Status::Error::NoDataReadFromSocket);
        }

        shutdown(sourceSocket, SHUT_RD);
        shutdown(destinationSocket, SHUT_WR);

        close(sniffingSocket);
        close(sourceSocket);
        close(destinationSocket);

        return status;
    }

    bool IsServerHelloMesasge(const char *buff) noexcept
    {
        // buff[66] - position of the TLS Content Type field. [0x16 - Handshake]/[0x17 - Application Data]
        // buff[71] - position of the Handshake Type [0x01 - ClienHello]/[0x02 - ServerHello]
        return ( static_cast<uint32_t>(buff[0]) == 0x16 ) && ( static_cast<uint32_t>(buff[5]) == 0x02 );
    }

    std::string GetDomainNameFromServerHello(const char *buffer) noexcept
    {
        auto domainNameSize = static_cast<uint32_t>(buffer[0x156]);
        char* domainName = new char[domainNameSize];
        memcpy(domainName,buffer+0x157,domainNameSize);
        std::string tmp(domainName);
        delete[] domainName;
        return tmp;
    }

}

