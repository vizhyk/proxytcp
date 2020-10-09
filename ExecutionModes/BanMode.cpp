#include <vector>
#include "BanMode.hpp"

namespace Proxy::ExecutionModes
{

    int32_t BanMode::Run(const ConnectionInfo& info) const noexcept
    {
        std::cout << "[Ban mode]\n";

        Utilities::Status status {};
        sockaddr_in socketData {};

        int32_t listeningSocket {};
//        int32_t newConnectionSocket {};

        pid_t parentPID;

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

                Utilities::Status transferStatus = TransferDataWithRestriction(newConnectionSocket, info.GetBannedHostName(), info.GetDestinationPort());
                if (transferStatus.Failed() && (transferStatus.Code() != static_cast<int32_t>(Utilities::Status::Error::BannedHostDataTransfer)))
                { PrintStatusAndTerminateProcess(transferStatus); }

                close(newConnectionSocket);

            });

            newConnectionThread.join();

//            threads.emplace_back(std::move(newConnectionThread));




//            parentPID = fork();
//            if(parentPID == -1)
//            {
//                status = Status(Status::Error::BadProcessFork);
//                PrintStatusAndTerminateProcess(status);
//            }
//
//
//            if(parentPID == 0)
//            {
//                std::cout << "[" << info.listeningPort_ << "->" << info.destinationPort_ << "]\n";
//
//                status = Ban::TransferDataWithRestriction(newConnectionSocket, info.bannedHostName_, info.destinationPort_);
//                if (status.Failed() && (status.Code() != static_cast<int32_t>(Status::Error::BannedHostDataTransfer)))
//                { PrintStatusAndTerminateProcess(status); }
//
//                close(newConnectionSocket);
//            }
//            close(newConnectionSocket);
        }

        return 0;
    }
} //namespace Proxy::ExecutionModes
