#include "Proxy.hpp"

// usage
// ./proxy [port:forward_from] [port:forward_to]


int main(int argc, char** argv)
{
    Proxy::Status status {};
    Proxy::ForwardingData fwd {};

    int32_t listeningSocket {};

    status = Proxy::InitForwardingData(argc, argv, fwd);
    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

    status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.listeningPort);
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
                std::cout << "[Transfering data from listening socket to destination]\n";
                status = Proxy::TransferData(forwardingSocket, clientSocket);
                if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
            }
            else
            {
                std::cout << "[Transfering data from destination to listeningSocket]\n";
                status = Proxy::TransferData(clientSocket, forwardingSocket);
                if(status.Failed()) { Proxy::PrintStatusAndExit(status); }
            }

            exit(static_cast<int32_t>(Proxy::Status::Success::Success));
        }

        close(clientSocket);
    }

    return 0;
}
