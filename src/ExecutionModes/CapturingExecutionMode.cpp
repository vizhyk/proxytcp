#include "ConnectionManager/SocketCapturingConnectionManager.hpp"
#include "CapturingExecutionMode.hpp"

namespace Proxy
{
    CapturingExecutionMode::CapturingExecutionMode(std::string outputFilePath)
        : m_outputFilePath(std::move(outputFilePath))
    {
        m_connectionManager = std::make_unique<SocketCapturingConnectionManager>();
    }

    Status CapturingExecutionMode::Run(uint16_t port) const noexcept
    {
        dynamic_cast<SocketCapturingConnectionManager*>(m_connectionManager.get())->SetOutputFilePath(m_outputFilePath);
        return m_connectionManager->ProcessConnections(port);
    }
}