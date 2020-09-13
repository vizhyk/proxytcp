#include "Proxy.hpp"
#include <getopt.h>
// usage
// ./proxy [port:forward_from] [port:forward_to]

//void TrackingModeFlow()
//{
//
//    int32_t listeningSocket {};
//
//    status = Proxy::InitForwardingData(argc, argv, fwd);
//    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
//
//    status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort);
//    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
//
//    bool waitingForConnection { true };
//    while(waitingForConnection)
//    {
//        int32_t clientSocket {};
//        pid_t parentPID {};
//
//        clientSocket = accept(listeningSocket, nullptr , nullptr);
//        if(clientSocket == -1)
//        {
//            status = Proxy::Status(Proxy::Status::Error::BadConnectionFromListeningSocket);
//            Proxy::PrintStatusAndExit(status);
//        }
//
//        parentPID = fork();
//        if(parentPID == -1)
//        {
//            status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
//            Proxy::PrintStatusAndExit(status);
//        }
//
//        if(parentPID == 0)
//        {
//            int32_t forwardingSocket {};
//            pid_t childPID {};
//
//            status = Proxy::CreateSocketForForwarding( forwardingSocket, fwd.destinationPort, fwd.hostName.c_str());
//
//            // child process forward traffic to the client,
//            // parent process forward traffic to the destination[port]
//            childPID = fork();
//            if(childPID == -1)
//            {
//                status = Proxy::Status(Proxy::Status::Error::BadProcessFork);
//                Proxy::PrintStatusAndExit(status);
//            }
//
//            if(childPID == 0)
//            {
//                std::cout << "[Transfering data from listening socket to destination]\n";
//                status = Proxy::TransferData(forwardingSocket, clientSocket);
//                if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
//            }
//            else
//            {
//                std::cout << "[Transfering data from destination to listeningSocket]\n";
//                status = Proxy::TransferData(clientSocket, forwardingSocket);
//                if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
//            }
//
//            exit(static_cast<int32_t>(Proxy::Status::Success::Success));
//        }
//
//        close(clientSocket);
//    }
//}


int main(int argc, char** argv)
{
    Proxy::InputArgs mode;

    Proxy::Status status {};
    Proxy::ForwardingData fwd {};


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
                        std::cout << "forwarding mode\n";
                        break;
                    }
                    case static_cast<int32_t>(Proxy::Mode::Tracking):
                    {
                        std::cout << "tracking mode\n";
                        break;
                    }
                    case static_cast<int32_t>(Proxy::Mode::Ban):
                    {
                        std::cout << "ban mode\n";
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



    return 0;
}
