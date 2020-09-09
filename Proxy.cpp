#include "Proxy.hpp"

namespace Proxy
{

    void InitForwardingData(int argc, char **argv, ForwardingData &fwd, Status &status) noexcept
    {
        if(argc < 3)
        {
            status = Status::IncorrectInputArguments;
            return;
        }

        if(strcmp(argv[1],argv[2]) == 0)
        {
            status = Status::IncorrectInputArguments;
            return;
        }

        fwd.listeningPort = std::strtoul(argv[1], nullptr, 10);

        if(fwd.listeningPort == 0L || errno == ERANGE)
        {
            status = Status::BadListeningPortConversion;
            return;
        }

        fwd.destinationPort = std::strtoul(argv[2], nullptr, 10);

        if(fwd.destinationPort == 0L || errno == ERANGE)
        {
            status = Status::BadDestinationPortConversion;
            return;
        }

    }

    int32_t CreateSocketOnListeningPort(uint16_t listeningPort, Status &status) noexcept
    {
        sockaddr_in socketData {};
        int32_t  listeningSocket {};

        listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

        if(listeningSocket == -1)
        {
            status = Status::BadListeningSocketInitializaton;
            return -1;
        }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));

        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(listeningSocket);

        if(bind(listeningSocket,reinterpret_cast<sockaddr*>(&socketData),sizeof(socketData)) == -1)
        {
            std::cout << "bad binding\n";
            exit(1);
        }

        if(listen(listeningSocket, 40) == -1)
        {
            std::cout << "bad listening\n";
            exit(1);
        }

        return listeningSocket;
    }

    void TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept
    {
        const int32_t BUFFER_SIZE = 4096;
        char buffer[BUFFER_SIZE];
        uint32_t totalBytesWritten {};
        int32_t bytesWritten {};
        int32_t bytesRead {};

        //bytesRead = read(sourceSocket, buffer, BUFFER_SIZE);

        while((bytesRead = read(sourceSocket, buffer, BUFFER_SIZE)) > 0)
        {
            totalBytesWritten = 0;

            while(totalBytesWritten < bytesRead)
            {
                int32_t writeSize = bytesRead - totalBytesWritten;
                bytesWritten = write(destinationSocket, buffer + totalBytesWritten, writeSize );

                if(bytesWritten == - 1)
                {
                    std::cout << "bad data forwarding(write)\n";
                    exit(-1);
                }

                totalBytesWritten += bytesWritten;
            }
        }

        if(bytesRead == -1)
        {
            std::cout << "bad socket read(read)\n";
            exit(-1);
        }

        shutdown(sourceSocket, SHUT_RD);
        shutdown(destinationSocket, SHUT_WR);

        close(sourceSocket);
        close(destinationSocket);

        exit(0);

    }

    int32_t CreateSocketForForwarding(uint16_t destinationPort,const char* hostName) noexcept
    {
        int16_t socketForForwarding {};
        hostent* destinationHost = nullptr;
        sockaddr_in destinationAddress {};

        destinationHost = gethostbyname(hostName);

        if(destinationHost == nullptr)
        {
            std::cout << "bad destination host";
            return 1;
        }

        memset(&destinationAddress, 0, sizeof(destinationAddress));

        destinationAddress.sin_family = AF_INET;

        memcpy(reinterpret_cast<char*>(destinationHost->h_addr),
               reinterpret_cast<char*>(&destinationAddress.sin_addr.s_addr),
               destinationHost->h_length);

        destinationAddress.sin_port = htons(destinationPort);

        socketForForwarding = socket(AF_INET, SOCK_STREAM, 0);

        if(socketForForwarding == -1)
        {
            std::cout << "bad socket for forwarding";
            return 1;
        }

        int16_t connectResult {};
        connectResult = connect(socketForForwarding, reinterpret_cast<sockaddr*>(&destinationAddress), sizeof(destinationAddress));

        if(connectResult == -1)
        {
            std::cout << "bad forwarding connection\n";
            return 1;
        }

        return socketForForwarding;
    }


}

