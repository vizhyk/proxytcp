#include <utility>

#include "CommandLineOptions.hpp"

namespace Proxy
{

    CommandLineOptions::CommandLineOptions(uint8_t executionMode, uint32_t port, std::string outputFilePath) noexcept
        : m_executionModeID(executionMode), m_port(port), m_outputFilePath(std::move(outputFilePath))
    {}

    uint32_t CommandLineOptions::GetExecutionModeID() const noexcept
    {
        return m_executionModeID;
    }

    uint32_t CommandLineOptions::GetPort() const noexcept
    {
        return m_port;
    }

    std::string CommandLineOptions::GetOutputFilePath() const noexcept
    {
        return m_outputFilePath;
    }

} //namespace Proxy