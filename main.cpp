#include "Proxy.hpp"

// usage
// ./main [port:forward_from] [port:forward_to]


int main(int argc, char** argv)
{
    Proxy::Status status {};
    Proxy::ForwardingData fwd {};

    int32_t listeningSocket {};

    status = Proxy::InitForwardingData(argv, fwd);
    if(status.Failed()) { Proxy::PrintStatusAndExit(status); }

    status = Proxy::CreateSocketOnListeningPort(listeningSocket, fwd.serverPort);
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

            exit(1);
        }

        if(parentPID == 0)
        {
            int32_t forwardingSocket {};
            pid_t childPID {};

            forwardingSocket = Proxy::CreateSocketForForwarding(fwd.destinationPort, fwd.hostName.c_str(),
                                                                <#initializer#>);

            childPID = fork();
            if(childPID == -1)
            {
                std::cout << "bad child PID\n";
                exit(1);
            }

            if(childPID == 0)
            {
                Proxy::TransferData(forwardingSocket, clientSocket);
            }
            else
            {
                Proxy::TransferData(clientSocket, forwardingSocket);
            }

            exit(0);
        }

        close(clientSocket);
    }



    return 0;
}
