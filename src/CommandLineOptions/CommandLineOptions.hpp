#ifndef PROXYTCP_COMMANDLINEOPTIONS_HPP
#define PROXYTCP_COMMANDLINEOPTIONS_HPP

#include <cstdint>
#include <getopt.h>
#include <string>

#include "src/ExecutionModeType/ExecutionModeType.hpp"

namespace Proxy
{
    class CommandLineOptions
    {
    public:
        enum class OptionID : uint32_t
        {
            Mode = 0x00,
            OutputFilePath = 0x01,
            ListeningPort = 0x02,
        };
    public:
        CommandLineOptions(uint32_t argc, char** argv) noexcept;

    public:
        uint8_t GetChosenExecutionModeID() const noexcept;
        uint16_t GetChosenPort() const noexcept;
        const std::string & GetChosenOutputFilePath() const noexcept;

    private:
        uint32_t m_port;
        uint8_t  m_executionMode;
        std::string m_outputFilePath;
    };

}

#endif //PROXYTCP_COMMANDLINEOPTIONS_HPP
