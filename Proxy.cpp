#include "Proxy.hpp"

namespace Proxy
{

    Status InitForwardingData(int32_t argc, char **argv, ForwardingData &fwd) noexcept
    {
        Status status {};

        if(argc < 3)
        {
            status = Status(Status::Error::IncorrectInputArguments);
            return status;
        }

        if(strcmp(argv[1],argv[2]) == 0)
        {
            status = Status(Status::Error::IncorrectInputArguments);
            return status;
        }

        fwd.listeningPort = std::strtoul(argv[1], nullptr, 10);
        if(fwd.listeningPort == 0L || errno == ERANGE)
        {
            status = Status(Status::Error::BadListeningPortConversion);
            return status;
        }

        fwd.destinationPort = std::strtoul(argv[2], nullptr, 10);
        if(fwd.destinationPort == 0L || errno == ERANGE)
        {
            status = Status(Status::Error::BadDestinationPortConversion);
            return status;
        }

        std::cout << "[hostname = " << fwd.hostName << "]\n";
        std::cout << "[destination port = " << fwd.destinationPort << "]\n";
        std::cout << "[server port = " << fwd.listeningPort << "]\n";

        return status;
    }

    Status CreateSocketOnListeningPort(int32_t &listeningSocket, int32_t listeningPort) noexcept
    {
        Status status {};

        sockaddr_in socketData {};

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


}

