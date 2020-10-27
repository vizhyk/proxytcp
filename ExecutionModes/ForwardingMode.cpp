#include "ForwardingMode.hpp"

namespace Proxy::ExecutionModes
{

    int32_t ForwardingMode::Run(const ConnectionInfo& info, ThreadPool<std::function<void()>>& threadPool) const noexcept
    {
        std::cout << "[Forwarding mode]\n";

        Utilities::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};

        std::vector<std::thread> threads;
        // listening on 8081
        status =  CreateSocketOnListeningPort(listeningSocket, info.GetListeningPort(), socketData);
        if(status.Failed()) { PrintStatusAndTerminateProcess(status); }

        bool waitingForConnection { true };
        while(waitingForConnection)
        {

            int32_t newConnectionSocket = accept(listeningSocket, nullptr , nullptr);
            if(newConnectionSocket == -1)
            {
                status = Utilities::Status(Utilities::Status::Error::BadConnectionFromListeningSocket);
                PrintStatusAndTerminateProcess(status);
            }

            std::thread newConnectionThread ([this,newConnectionSocket, &info = std::as_const(info)] () {

                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[" << info.GetListeningPort() << "->" << info.GetDestinationPort() << "]\n";

                int32_t destinationSocket {};

                const Utilities::Status status = CreateForwardingSocketByHostname(destinationSocket,
                                                                                  info.GetDestinationPort(),
                                                                                  info.GetHostName());
                if(status.Failed())
                { PrintStatusAndTerminateProcess(status); }

                const Utilities::Status transferStatus = TransferData(newConnectionSocket, destinationSocket);
                if(transferStatus.Failed())
                { PrintStatusAndTerminateProcess(transferStatus); }

                close(newConnectionSocket);

            });

            threads.emplace_back(std::move(newConnectionThread));
        }

        return 0;
    }
} //namespace Proxy::ExecutionModes