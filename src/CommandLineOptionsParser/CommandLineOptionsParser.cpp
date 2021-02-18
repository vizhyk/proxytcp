#include "CommandLineOptionsParser.hpp"

#include <iostream>
#include <cstring>

namespace Proxy
{

    CommandLineOptionsParser::CommandLineOptionsParser(uint32_t argc, char** argv) noexcept
    {
        const option longOptions[] = {
                {"mode", required_argument, nullptr , static_cast<int32_t>(OptionIDs::Mode) },
                {"port", required_argument, nullptr , static_cast<int32_t>(OptionIDs::ListeningPort) },
                {"output", required_argument, nullptr , static_cast<int32_t>(OptionIDs::Output) },
        };

        int32_t result {};
        int32_t optionIndex {};

        uint32_t tmpListeningPort {};

        while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
        {
            switch (result)
            {
                case static_cast<int32_t>(OptionIDs::Mode):
                {
                    std::cout << "[Option Found: mode]\n";
                    if(strcmp(optarg, "default") == 0)
                    {
                        m_modeType = ExecutionModeType::DefaultForward;
                    }
                    if(strcmp(optarg, "capture") == 0)
                    {
                        m_modeType = ExecutionModeType::Capture;
                    }
                    if(strcmp(optarg, "replay") == 0)
                    {
                        m_modeType = ExecutionModeType::Replay;
                    }

                    std::cout << "[invalid mode]\n";
                    exit(1);
                }

            }
        }

        m_cmdOptions = CommandLineOptions{tmpDestinationPort, tmpListeningPort, tmpHostName, tmpBannedHostName};

    }

    CommandLineOptions CommandLineOptionsParser::GetCommandLineOptions() const noexcept
    {
        return m_cmdOptions;
    }

    uint8_t CommandLineOptionsParser::GetExecutionModeType() const noexcept
    {
        return m_modeType;
    }
}