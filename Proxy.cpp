#include <netinet/ip.h>
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

    Status ParseInputArguments(int32_t argc, char **argv, ForwardingData &fwd, FunctionPointer* fptr) noexcept
    {
        Status status {};

        const option longOptions[] = {
                {"mode", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Mode) },
                {"domain", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Domain) },
                {"lp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::ListeningPort) },
                {"dp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::DestinationPort) },
        };

        int32_t result;
        int32_t optionIndex;

        while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
        {
            switch (result)
            {
                case static_cast<int32_t>(Proxy::InputArgs::Mode):
                {
                    std::cout << "found: mode\n";
                    switch (strtol(optarg, nullptr,10))
                    {
                        case static_cast<int32_t>(Proxy::Mode::Forwarding):
                        {
                            *fptr = (FunctionPointer)StartForwardingMode;
                            break;
                        }
                        case static_cast<int32_t>(Proxy::Mode::Tracking):
                        {
                            std::cout << "tracking mode\n";
                            *fptr = (FunctionPointer)Tracking::StartTrackingMode;
                            break;
                        }
                        case static_cast<int32_t>(Proxy::Mode::Ban):
                        {
                            std::cout << "ban mode\n";
                            *fptr = (FunctionPointer)Ban::StartBanMode;
                            break;
                        }
                        default:
                        {
                            std::cout << "invalid mode\n";
                            exit(1);
                        }
                    }
                    break;
                }
                case static_cast<int32_t>(Proxy::InputArgs::Domain):
                {
                    fwd.hostName = std::string(optarg);
                    std::cout << "domain: " << fwd.hostName << "\n";
                    break;
                }
                case static_cast<int32_t>(Proxy::InputArgs::ListeningPort):
                {
                    fwd.listeningPort = strtol(optarg, nullptr,10);
                    std::cout << "listening port: " << fwd.listeningPort << "\n";
                    break;
                }
                case static_cast<int32_t>(Proxy::InputArgs::DestinationPort):
                {
                    fwd.destinationPort = strtol(optarg, nullptr,10);
                    std::cout << "destination port: " << fwd.destinationPort << "\n";
                    break;
                }
                default:
                {
                    std::cout << "Invalid arguments\n";
                    break;
                }
            }
        }


        return status;
    }

    void StartForwardingMode(const ForwardingData &fwd) noexcept
    {
        Proxy::Status status {};
        sockaddr_in socketData {};
        std::cout << "forwarding\n";

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
        std::cout << "Tracking mode\n";

        Proxy::Status status {};
        sockaddr saddr {};
        auto socketDataSize = sizeof(saddr);


        int32_t rawSocket = socket(AF_INET , SOCK_RAW , IPPROTO_TCP);
        if(rawSocket < 0)
        {
            printf("Socket Error\n");
            exit(1);
        }

        char buff[4096];

        while(1)
        {

            //Receive a packet
            auto recievedPackets = recvfrom(rawSocket , buff , 4096 , 0 , &saddr , (socklen_t*)&socketDataSize);
            if(recievedPackets <0 )
            {
                printf("Recvfrom error , failed to get packets\n");
                exit(1);
            }

            unsigned short iphdrlen;

            iphdr *iph = (struct iphdr *)( buff  + sizeof(ethhdr) );
            iphdrlen = iph->ihl*4;

            tcphdr *tcph=(struct tcphdr*)(buff + iphdrlen + sizeof(ethhdr));

            int header_size =  sizeof(ethhdr) + iphdrlen + tcph->doff*4;

            if(iph->protocol == 6)
            {
                printf("############# tcp header #########\n");
                for(int i = header_size; i < 100; ++i)
                {
                    printf("%02x ",(unsigned char)buff[i]);
                    if(i!=0 && i%16==0)
                    {
                        printf("\n");
                    }
                }

            }
        }



    }
}

namespace Proxy::Ban
{
    void StartBanMode(const ForwardingData &fwd) noexcept
    {
        std::cout << "Ban mode\n";
    }
}
