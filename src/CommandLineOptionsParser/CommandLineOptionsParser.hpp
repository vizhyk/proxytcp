#ifndef PROXYTCP_COMMANDLINEOPTIONSPARSER_HPP
#define PROXYTCP_COMMANDLINEOPTIONSPARSER_HPP

#include "src/CommandLineOptions/CommandLineOptions.hpp"
#include "src/ExecutionModeType/ExecutionModeType.hpp"
#include <cstdint>
#include <getopt.h>

namespace Proxy
{
    class CommandLineOptionsParser
    {
    public:
        enum class OptionIDs : uint32_t
        {
            Mode = 0x00,
            Output = 0x01,
            ListeningPort = 0x02,

        };
    public:
        CommandLineOptionsParser(uint32_t argc, char** argv) noexcept;
    public:
        CommandLineOptions GetCommandLineOptions() const noexcept;
        uint8_t GetExecutionModeType() const noexcept;

    private:
        CommandLineOptions m_cmdOptions;
        uint8_t m_modeType;
    };

}

#endif //PROXYTCP_COMMANDLINEOPTIONSPARSER_HPP
