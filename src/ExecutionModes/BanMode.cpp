#include "BanMode.hpp"

namespace Proxy::ExecutionModes
{

    int32_t BanMode::Run(const ConnectionInfo& info,  ThreadPool<std::function<void()>>& threadPool) const noexcept
    {
        std::cout << "[Ban mode]\n";

        Utilities::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};

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

            threadPool.AddWork([this,newConnectionSocket, &info = std::as_const(info)] () {


                std::cout << "[Thread " << std::this_thread::get_id() << "]" << "\t\t[" << info.GetListeningPort() << "->" << info.GetDestinationPort() << "]\n";

                const Utilities::Status transferStatus = TransferDataWithRestriction(newConnectionSocket, info.GetBannedHostName(), info.GetDestinationPort());
                if(transferStatus.Failed() && transferStatus != Utilities::Status::Error::BadConnectionHostDomainName)
                { PrintStatusAndTerminateProcess(transferStatus); }

                close(newConnectionSocket);

            });


        }

        return 0;
    }
} //namespace Proxy::ExecutionModes
