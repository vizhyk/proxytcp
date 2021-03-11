#include "CommandLineOptions.hpp"

#include <iostream>
#include <cstring>

namespace Proxy
{

    CommandLineOptions::CommandLineOptions(uint32_t argc, char** argv) noexcept
    {
        if(argc < 3)
        {
            std::cout << "[Not enough args.]\n";
            std::cout << "[Usage]: ./proxytcp --mode [default/capture/replay] --port [port] --output [filename]";
        }

        const option longOptions[] = {
                {"mode", required_argument, nullptr , static_cast<int32_t>(OptionID::Mode) },
                {"port", required_argument, nullptr , static_cast<int32_t>(OptionID::ListeningPort) },
                {"output", required_argument, nullptr , static_cast<int32_t>(OptionID::OutputFilePath) },
        };

        int32_t result = 0;
        int32_t optionIndex = 0;

        while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
        {
            switch (result)
            {
                case static_cast<int32_t>(OptionID::Mode):
                {
                    std::cout << "[Option Found: mode]\n";
                    if(strcmp(optarg, "default") == 0)
                    {
                        m_executionMode = ExecutionModeType::DefaultForward;
                        break;
                    }
                    if(strcmp(optarg, "capture") == 0)
                    {
                        m_executionMode = ExecutionModeType::Capture;
                        break;
                    }
                    if(strcmp(optarg, "replay") == 0)
                    {
                        m_executionMode = ExecutionModeType::Replay;
                        break;
                    }

                    std::cout << "[invalid mode]\n";
                    exit(1);
                }

                case static_cast<int32_t>(OptionID::OutputFilePath):
                {
                    std::cout << "[Option Found: output]\n";
                    m_outputFilePath = optarg;
                }

                case static_cast<int32_t>(OptionID::ListeningPort):
                {
                    std::cout << "[Option Found: port]\n";
                    m_port = std::strtol(optarg, nullptr,  10);
                }

                default:
                    break;

            }
        }
        std::cout << "[" << static_cast<uint32_t>(m_executionMode) << " : " << m_outputFilePath << " : " << m_port << "]\n";
    }

    uint8_t CommandLineOptions::GetChosenExecutionMode() const noexcept
    {
        return m_executionMode;
    }

    std::string CommandLineOptions::GetChosenOutputFilePath() const noexcept
    {
        return m_outputFilePath;
    }

    uint16_t CommandLineOptions::GetChosenPort() const noexcept
    {
        return m_port;
    }
}