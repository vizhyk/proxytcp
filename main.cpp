#include "Proxy.hpp"

// usage
// ./main [port:forward_from] [port:forward_to]


int main(int argc, char** argv)
{
    Proxy::Status status {};
    Proxy::ForwardingData fwd {};

    int32_t clientSocket {};
    int32_t serverSocket {};

    pid_t parentPID {};

    Proxy::InitForwardingData(argc,argv,fwd,status);

    if(status == Proxy::Status::IncorrectInputArguments ||
       status == Proxy::Status::BadListeningPortConversion ||
       status == Proxy::Status::BadDestinationPortConversion)
    {
        std::cout << "err step1\n";
        exit(1);
    }

    serverSocket = Proxy::CreateSocketOnListeningPort(fwd.listeningPort, status);


    clientSocket = accept(serverSocket, nullptr , nullptr);

    if(clientSocket == -1)
    {
        std::cout << "Bad creating client Socket\n";
        exit(1);
    }

    parentPID = fork();

    if(parentPID == -1)
    {
        std::cout << "bad parent pid\n";
        exit(1);
    }

    if(parentPID == 0)
    {
        int32_t forwardingSocket {};
        pid_t childPID {};

        forwardingSocket = Proxy::CreateSocketForForwarding(fwd.destinationPort, fwd.hostName.c_str());

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

    return 0;
}
