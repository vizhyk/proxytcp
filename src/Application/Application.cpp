#include "Application.hpp"
#include "Connection/SocketConnection.hpp"
#include "ConnectionManager/SocketConnectionManager.hpp"
#include "ConnectionManager/SocketCapturingConnectionManager.hpp"
#include "ExecutionModeType/ExecutionModeType.hpp"

namespace Proxy
{
    Application::Application(uint16_t port) noexcept
        : m_port(port)
    {
    }

    Status Application::Run() noexcept
    {
        return (m_connectionManager) ? m_connectionManager->ProcessConnections(m_port) : Status(-1);

//        auto status = m_connectionManager.ProcessConnections();
//        // LCOV_EXCL_START
//        return status.Code();
//        // LCOV_EXCL_STOP
    }

    Status Application::InitConnectionManager(uint8_t executionModeID, const std::string& outputFilePath)
    {
        switch (executionModeID)
        {
            case ExecutionModeType::DefaultForward:
                m_connectionManager = std::make_unique<SocketConnectionManager>();
                break;
            case ExecutionModeType::Capture:
                m_connectionManager = std::make_unique<SocketCapturingConnectionManager>(outputFilePath);
                break;
            case ExecutionModeType::Replay:
            default:
                return Status(Status::Error::NoAvailableModeChosen);
                break;

        }

        if(m_connectionManager)
            return Status();
        else
            return Status(Status::Error::ConnectionManagerIsNotInitialized);
    }


}


