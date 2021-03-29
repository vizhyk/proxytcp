#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP

#include "SocketConnectionManager.hpp"

namespace Proxy
{
    class SocketCapturingConnectionManager : public SocketConnectionManager
    {
    public:
        explicit SocketCapturingConnectionManager(const std::string& outputFilePath) noexcept;
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTIONMANAGER_HPP
