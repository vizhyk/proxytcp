#include "Application.hpp"
#include "Connection/ClientConnection.hpp"
#include "Connection/ServerConnection.hpp"


namespace Proxy
{
    int32_t Application::Run() noexcept
    {
        auto status = m_connectionManager.ProcessConnections();

        return status.Code();
    }
}


