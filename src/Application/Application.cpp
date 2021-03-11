#include "Application.hpp"
#include "Connection/SocketConnection.hpp"


namespace Proxy
{
    Application::Application(uint8_t executionModeID, uint16_t port, const std::string& outputFilePath) noexcept
        : m_port(port)
    {

        switch (executionModeID)
        {
            case ExecutionModeType::DefaultForward:
                m_executionMode = std::make_unique<DefaultExecutionMode>();
                break;
            case ExecutionModeType::Capture:
                m_executionMode = std::make_unique<CapturingExecutionMode>(outputFilePath);
                break;
            case ExecutionModeType::Replay:
            default:
                break;

        }
    }

    Status Application::Run() noexcept
    {
        return (m_executionMode) ? m_executionMode->Run(m_port) : Status(-1);

//        auto status = m_connectionManager.ProcessConnections();
//        // LCOV_EXCL_START
//        return status.Code();
//        // LCOV_EXCL_STOP
    }


}


