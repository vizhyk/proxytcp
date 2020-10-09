#include "TrackingMode.hpp"

namespace Proxy::ExecutionModes
{

    int32_t TrackingMode::Run(const ConnectionInfo& info) const noexcept
    {
        std::cout << "[Tracking mode]\n";

        Utilities::Status status {};
        sockaddr_in socketData {};

        int32_t socketDataSize = sizeof(socketData);
        int32_t listeningSocket {};

        int32_t recievedData {};

        const int32_t BUFFER_SIZE = 4096;
        char  buffer [BUFFER_SIZE];

        status = CreateSocketOnListeningPort(listeningSocket, info.GetListeningPort(), socketData);
        if(status.Failed()) { PrintStatusAndTerminateProcess(status); }

        bool waitingForConnection {true};
        while(waitingForConnection)
        {
            int32_t newConnectionSocket {};

            newConnectionSocket = accept(listeningSocket, nullptr, nullptr);

            if(newConnectionSocket == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadConnectionFromListeningSocket);
                PrintStatusAndTerminateProcess(status);
            }
            std::cout << "new connection found.\n";

            while((recievedData = read(newConnectionSocket,buffer,BUFFER_SIZE)) > 0)
            { close(newConnectionSocket);
                PrintRecievedData(buffer,75);

                if(IsClientHelloMesasge(buffer, Utilities::Offsets::TLS::TLS_DATA))
                {
                    std::string domain = GetDomainNameFromTCPPacket(buffer, Utilities::Offsets::TLS::TLS_DATA);
                    std::cout << "You have been connected with: " << domain << "\n";
                }
            }

            if(recievedData == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadRecievingDataFromSocket);
                PrintStatusAndTerminateProcess(status);
            }

        }
    }
} //namespace Proxy::ExecutionModes