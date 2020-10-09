#ifndef PROXYTCP_COMMANDLINEOPTIONS_HPP
#define PROXYTCP_COMMANDLINEOPTIONS_HPP

#include "ConnectionInfo/ConnectionInfo.hpp"
#include "ExecutionModes/ExecutionMode.hpp"
#include <cstdint>
#include <getopt.h>

namespace Proxy
{

    class CommandLineOptions
    {
    public:
        enum class InputArgs : uint32_t
        {
            Mode = 0,
            Domain = 1,
            ListeningPort = 2,
            DestinationPort = 3,
            BannedDomain = 4,
        };
    public:
        CommandLineOptions(uint32_t argc, char** argv) noexcept;
    public:
        ConnectionInfo GetConnectionInfo() const noexcept;
        ExecutionModes::ExecutionModeType GetExecutionModeType() const noexcept;


    private:
        ConnectionInfo info;
        ExecutionModes::ExecutionModeType modeType;
    };

}

#endif //PROXYTCP_COMMANDLINEOPTIONS_HPP
