#ifndef PROXYTCP_COMMANDLINEOPTIONS_HPP
#define PROXYTCP_COMMANDLINEOPTIONS_HPP

#include <cstdint>
#include <string>

namespace Proxy
{
    class CommandLineOptions
    {
    public:
        CommandLineOptions() noexcept = default;
        CommandLineOptions(uint8_t executionMode, uint32_t port, std::string  outputFilePath) noexcept;

    public:
        uint32_t GetExecutionModeID() const noexcept;
        uint32_t GetPort() const noexcept;
        std::string GetOutputFilePath() const noexcept;

    private:
        uint8_t m_executionModeID;
        uint32_t m_port;
        std::string m_outputFilePath;
    };

} //namespace Proxy




#endif //PROXYTCP_COMMANDLINEOPTIONS_HPP
