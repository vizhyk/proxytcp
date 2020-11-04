#include "CommandLineOptions.hpp"

namespace Proxy
{

    CommandLineOptions::CommandLineOptions(uint32_t argc, char** argv) noexcept
    {
        const option longOptions[] = {
                {"mode", required_argument, nullptr , static_cast<int32_t>(InputArgs::Mode) },
                {"domain", required_argument, nullptr , static_cast<int32_t>(InputArgs::Domain) },
                {"lp", required_argument, nullptr , static_cast<int32_t>(InputArgs::ListeningPort) },
                {"dp", required_argument, nullptr , static_cast<int32_t>(InputArgs::DestinationPort) },
                {"ban", required_argument, nullptr , static_cast<int32_t>(InputArgs::BannedDomain) },
        };

        int32_t result {};
        int32_t optionIndex {};

        uint32_t tmpListeningPort {};
        uint32_t tmpDestinationPort {};
        std::string tmpHostName {};
        std::string tmpBannedHostName {};

        while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
        {
            switch (result)
            {
                case static_cast<int32_t>(InputArgs::Mode):
                {
                    std::cout << "[found: mode]\n";
                    switch (strtol(optarg, nullptr,10))
                    {
                        case static_cast<int32_t>(ExecutionModes::ExecutionModeType::Forwarding):
                        {
                            modeType = ExecutionModes::ExecutionModeType::Forwarding;
                        }
                        case static_cast<int32_t>(ExecutionModes::ExecutionModeType::Tracking):
                        {
                            modeType = ExecutionModes::ExecutionModeType::Tracking;
                        }
                        case static_cast<int32_t>(ExecutionModes::ExecutionModeType::Ban):
                        {
                            modeType = ExecutionModes::ExecutionModeType::Ban;
                        }
                        default:
                        {
                            std::cout << "[invalid mode]\n";
                            exit(1);
                        }
                    }
                    break;
                }
                case static_cast<int32_t>(InputArgs::Domain):
                {
                    tmpHostName = std::string(optarg);
                    std::cout << "domain: " << tmpHostName << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::BannedDomain):
                {
                    tmpBannedHostName = std::string(optarg);
                    std::cout << "banned: " << tmpBannedHostName << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::ListeningPort):
                {
                    tmpListeningPort = strtol(optarg, nullptr, 10);
                    std::cout << "listening port: " << tmpListeningPort << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::DestinationPort):
                {
                    tmpDestinationPort = strtol(optarg, nullptr, 10);
                    std::cout << "destination port: " << tmpDestinationPort << "\n";
                    break;
                }
                default:
                {
                    std::cout << "Invalid arguments\n";
                    break;
                }
            }
        }

        info = ConnectionInfo{tmpDestinationPort,tmpListeningPort,tmpHostName,tmpBannedHostName};

    }
}