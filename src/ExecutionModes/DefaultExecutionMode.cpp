#include "ConnectionManager/SocketConnectionManager.hpp"
#include "DefaultExecutionMode.hpp"

namespace Proxy
{

    DefaultExecutionMode::DefaultExecutionMode()
    {
        m_connectionManager = std::make_unique<SocketConnectionManager>();
    }

    Status DefaultExecutionMode::Run(uint16_t port) const noexcept
    {
        return m_connectionManager->ProcessConnections(port);
    }
}