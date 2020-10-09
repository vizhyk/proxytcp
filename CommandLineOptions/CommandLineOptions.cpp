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

        int32_t result;
        int32_t optionIndex;

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
                    .hostName = std::string(optarg);
                    std::cout << "domain: " << fwd.hostName_ << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::BannedDomain):
                {
                    fwd.bannedHostName_ = std::string(optarg);
                    std::cout << "banned: " << fwd.bannedHostName_ << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::ListeningPort):
                {
                    fwd.listeningPort_ = strtol(optarg, nullptr, 10);
                    std::cout << "listening port: " << fwd.listeningPort_ << "\n";
                    break;
                }
                case static_cast<int32_t>(InputArgs::DestinationPort):
                {
                    fwd.destinationPort_ = strtol(optarg, nullptr, 10);
                    std::cout << "destination port: " << fwd.destinationPort_ << "\n";
                    break;
                }
                default:
                {
                    std::cout << "Invalid arguments\n";
                    break;
                }
            }
        }

    }
}