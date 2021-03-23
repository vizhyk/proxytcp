#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP

#include "SocketConnectionManager.hpp"

namespace Proxy
{
    class SocketCapturingConnectionManager : public SocketConnectionManager
    {
    public:
        explicit SocketCapturingConnectionManager(const std::string& outputFilePath) noexcept;

        Status FindPipelineAndPerformTransaction(int32_t sockfd) override;

    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
