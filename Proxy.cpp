#include "Proxy.hpp"

namespace Proxy
{

    Status InitForwardingData(char **argv, ForwardingData &fwd) noexcept
    {
//        if(argc < 3)
//        {
//            status = Status::IncorrectInputArguments;
//            return;
//        }
//
//        if(strcmp(argv[1],argv[2]) == 0)
//        {
//            status = Status::IncorrectInputArguments;
//            return;
//        }



//        if(fwd.serverPort == 0L || errno == ERANGE)
//        {
//            status = Status::BadListeningPortConversion;
//            return;
//        }


//        if(fwd.destinationPort == 0L || errno == ERANGE)
//        {
//            status = Status::BadDestinationPortConversion;
//            return;
//        }
        fwd.destinationPort = std::strtoul(argv[2], nullptr, 10);
        fwd.serverPort = std::strtoul(argv[1], nullptr, 10);
        std::cout << "[destination port = " << fwd.destinationPort << "]\n";
        std::cout << "[server port = " << fwd.serverPort << "]\n";

    }

    Status CreateSocketOnListeningPort(int32_t &listeningPort, uint16_t serverPort) noexcept
    {
        sockaddr_in socketData {};
        int32_t  serverSocket {};

        listeningPort = socket(AF_INET, SOCK_STREAM, 0);

        if(listeningPort == -1)
        {
            status = Status::BadserverSocketInitializaton;
            exit(1);
            //return -1;
        }

        memset(reinterpret_cast<char*>(&socketData),0,sizeof(socketData));
//        bzero((char *) &socketData, sizeof(socketData));
        socketData.sin_family = AF_INET;
        socketData.sin_addr.s_addr = INADDR_ANY;
        socketData.sin_port = htons(serverPort);

        if(bind(listeningPort, reinterpret_cast<sockaddr*>(&socketData), sizeof(socketData)) == -1)
        {
            std::cout << "bad binding\n";
            exit(1);
        }

        if(listen(listeningPort, 40) == -1)
        {
            std::cout << "bad server\n";
            exit(1);
        }

        return serverSocket;
    }

    void TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept
    {
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

    void PrintStatusAndExit(const Status &status) noexcept
    {
        std::cout << "[Status code: " << status.Code() << "]\n";
        exit(status.Code());
    }

    Status
    CreateSocketForForwarding(uint16_t destinationPort, const char *hostName, int32_t &socketForForwarding) noexcept
    {
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

        memcpy(reinterpret_cast<char*>(&destinationAddress.sin_addr.s_addr),
               reinterpret_cast<char*>(destinationHost->h_addr),
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


    }


}

