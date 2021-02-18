#include "Application.hpp"
#include "Connection/SocketConnection.hpp"


namespace Proxy
{
    int32_t Application::Run() noexcept
    {
        auto status = m_connectionManager.ProcessConnections();
        // LCOV_EXCL_START
        return status.Code();
        // LCOV_EXCL_STOP
    }
}


