#include "ExecutionMode.hpp"


namespace Proxy::ExecutionModes
{

    Utilities::Status ExecutionMode::CreateSocketForForwarding
    (int32_t& socketForForwarding, int32_t destinationPort, const char* hostName) const noexcept
    {
        Utilities::Status status {};

        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        destinationHost = gethostbyname(hostName);

        if(destinationHost == nullptr)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadDestinationHost);
            PrintStatusAndTerminateProcess(status);
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
            status =  Utilities::Status( Utilities::Status::Error::BadForwardingSocketCreation);
            PrintStatusAndTerminateProcess(status);
        }

        auto connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));

        if(connectResult == -1)
        {
            status =  Utilities::Status( Utilities::Status::Error::BadConnectionSocketToAddress);
            PrintStatusAndTerminateProcess(status);
        }

        return status;
    }

    Utilities::Status ExecutionMode::CreateSocketOnListeningPort
    (int32_t& listeningSocket, int32_t listeningPort, sockaddr_in& socketData) const noexcept
    {
        Utilities::Status status {};

        listeningSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if(listeningSocket == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::BadListeningSocketInitializaton);
            return status;
        }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));

        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(listeningPort);

        if(bind(listeningSocket, reinterpret_cast<sockaddr*>(&socketData), sizeof(socketData)) == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::BadBindListeningPortToSocket);
            return status;
        }

        if(listen(listeningSocket, 5) == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::MarkSocketPassive);
            return status;
        }

        return status;
    }

    Utilities::Status ExecutionMode::TransferData
    (int32_t sourceSocket, int32_t destinationSocket) const noexcept
    {
        Utilities::Status status {};

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
                    status = Utilities::Status(Utilities::Status::Error::BadDataWrittenOnForwarding);
                    return status;
                }

                totalBytesWritten += bytesWritten;
            }
        }

        if(bytesRead == -1)
        {
            status = Utilities::Status(Utilities::Status::Error::NoDataReadFromSocket);
        }

        shutdown(sourceSocket, SHUT_RD);
        shutdown(destinationSocket, SHUT_WR);

        close(sourceSocket);
        close(destinationSocket);

        return status;
    }

    Utilities::Status ExecutionMode::TransferDataWithRestriction
    (int32_t listeningSocket, const std::string& bannedHostname, int32_t destinationPort) const noexcept
    {
        const int32_t BUFFER_SIZE = 4096;
        char  buffer [BUFFER_SIZE];

        Utilities::Status status {};

        int32_t destinationSocket {};
        int32_t bytesSended {};
        int32_t bytesRead {};
        uint32_t totalBytesSended {};

        bool connectionIsAllowed = false;

        std::string connectedHostDomainName;

        //recieving ALL data that come to our listenignSocket
        while( (bytesRead = recv(listeningSocket , buffer , BUFFER_SIZE , 0)) > 0 )
        {
            std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[" << bytesRead << "b Client->Server]\n";
            if(IsClientHelloMesasge(buffer, Utilities::Offsets::TLS::TLS_DATA) && bytesRead > 6)
            {
                connectedHostDomainName = GetDomainNameFromTCPPacket(buffer, Utilities::Offsets::TLS::TLS_DATA);
                if( connectedHostDomainName == bannedHostname)
                {
                    std::cout << "Connection refused!\n";
                    status = Utilities::Status::Success::BannedHostConnectionRefused;
                    return status;
                }

                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[ClientHello: " << connectedHostDomainName << "]\n";
                // if host is not banned - allow connection
                connectionIsAllowed = true;

                status = CreateSocketForForwarding(destinationSocket, destinationPort, connectedHostDomainName.c_str());
                if(status.Failed()) { PrintStatusAndTerminateProcess(status); }
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
                        status = Utilities::Status(Utilities::Status::Error::BadDataWrittenOnForwarding);
                        return status;
                    }

                    totalBytesSended += bytesSended;

                    auto recievedFromServer = recv(destinationSocket, buffer, BUFFER_SIZE, 0);
                    if(recievedFromServer == -1)
                    {
                        status = Utilities::Status(Utilities::Status::Error::BadRecievingDataFromSocket);
                        return status;
                    }

                    std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[" << recievedFromServer << "b Server->Proxy]\n";

                    auto sendedFromServer = send(listeningSocket, buffer, recievedFromServer, 0);
                    if(sendedFromServer == -1)
                    {
                        status = Utilities::Status(Utilities::Status::Error::BadSendingDataToServer);
                        return status;
                    }

                    std::cout << "[Thread " << std::this_thread::get_id() << "]" <<"\t\t[" << sendedFromServer << "b Proxy->Client]\n";
                }
            }
        }
    }

    std::string ExecutionMode::GetDomainNameFromTCPPacket
    (const char* buffer, uint32_t offset) const noexcept
    {
        auto domainNameSize = static_cast<uint32_t>(buffer[Utilities::Offsets::TLS::SNI_SIZE - offset]);
        auto domainName = new char[domainNameSize];

        memcpy(domainName,buffer + (Utilities::Offsets::TLS::SNI - offset), domainNameSize);

        std::string tmp(domainName);
        delete[] domainName;

        return tmp;
    }

    bool ExecutionMode::IsClientHelloMesasge
    (const char* buff, int32_t offset) const noexcept
    {
        // offset is used for compatibility with raw sockets when they need.

        // buff[66] - position of the TLS Content Type field. [0x16 - Handshake]/[0x17 - Application Data]
        // buff[71] - position of the Handshake Type [0x01 - ClienHello]/[0x02 - ServerHello]
        return ( static_cast<uint32_t>(buff[Utilities::Offsets::TLS::MESSAGE_TYPE - offset]) == 0x16 ) &&
               ( static_cast<uint32_t>(buff[Utilities::Offsets::TLS::HANDSHAKE_TYPE - offset]) == 0x01 );
    }

    void ExecutionMode::PrintStatusAndTerminateProcess
    (const Utilities::Status& status) const noexcept
    {
        std::cout << "[Status code: " << status.Code() << " | " << strerror(errno) << "]\n";
        exit(status.Code());
    }

    void ExecutionMode::PrintRecievedData
    (const char* buffer, uint32_t size) const noexcept
    {
        printf("\n\n=============\n\n");
        for(int32_t i = 0; i < size; ++i)
        {
            if( (i%8 == 0) && (i%16 !=0) )
                printf("\t");

            if(i%16 == 0)
                printf("\n");

            printf("%02x ", static_cast<uint8_t>(buffer[i]));
        }
        printf("\n\n=============\n\n");
    }
} //namespace Proxy::ExecutionModes