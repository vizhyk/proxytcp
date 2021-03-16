#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP

#include "SocketConnectionManager.hpp"

namespace Proxy
{
    class SocketCapturingConnectionManager : public SocketConnectionManager
    {
    public:
        explicit SocketCapturingConnectionManager(std::string  outputFilePath) noexcept;

        Status FindPipelineAndPerformTransaction(int32_t sockfd) override;

    private:
        std::string m_outputFilePath;
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
