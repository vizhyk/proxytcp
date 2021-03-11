#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP

#include "SocketConnectionManager.hpp"

namespace Proxy
{
    class SocketCapturingConnectionManager : public SocketConnectionManager
    {
    public:
        SocketCapturingConnectionManager() noexcept;

        Status ProcessConnections(uint16_t port) override;

        void SetOutputFilePath(std::string outputFilePath) noexcept;
    private:
        std::string m_outputFilePath;
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
