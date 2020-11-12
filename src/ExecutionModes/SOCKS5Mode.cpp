
#include "SOCKS5Mode.hpp"

#include <csignal>

namespace Proxy::ExecutionModes
{

    int32_t SOCKS5Mode::Run(const ConnectionInfo& info, ThreadPool<std::function<void()>>& threadPool) const noexcept
    {
        std::cout << "[SOSCKS5 Mode]\n";
        signal(SIGPIPE, SIG_IGN);

        Utilities::Status status {};
        ConnectionManager connectionManager {};
        int32_t listeningSocket {};

        connectionManager.ProcessConnections(info);

        return 0;
    }

}