#include "Proxy.hpp"
/*      Quick Reference:
 *
 *      There're 3 modes according to the task:
 *          - Forwarding(item 3)
 *          - Tracking(item 4. Bonus A)
 *          - Ban(item 5. Bonus B)
 *
 *      There're following LONG options:
 *      --mode [0-2]
 *          0 - Forwarding
 *          1 - Tracking
 *          2 - Ban
 *      --domain [coolsite.io]
 *          Ban mode: is used as domain that is ALLOWED to connect
 *          Tracking: To track all client hello messages related to that  host.
 *      --ban [badsite.oi]
 *          Is Used only in Ban mode as a host that is NOT allowed to connect
 *      --lp [8081]
 *          listening port
 *      --dp [8080]
 *          destination port
 *
 *     There're configurations according to the task
 *
 *     Run ./compile.sh
 *     Forwarding:
 *          ./proxytcp --mode 0 --domain coolsite.io --lp 8081 --dp 8080
 *
 *     Tracking:
 *          ./proxytcp --mode 1
 *          Then surf
 *
 *     Ban:
 *          ./proxytcp --mode 2 --domain coolsite.io --ban badsite.io --lp 8081 --dp 8080
 *
 */

void ParseInputArguments(int32_t argc, char **argv, Proxy::ForwardingData &fwd, Proxy::ModeFunctionPointer* fptr) noexcept;

int main(int argc, char** argv)
{
    Proxy::ForwardingData fwd {};

    // it holds address of a function that depends on mode chosen by user.[forwarding/tracking(extension A)/ban(Extension B)
    Proxy::ModeFunctionPointer fptr = nullptr;

    ParseInputArguments(argc,argv,fwd,&fptr);

    fptr(fwd);

    return 0;
}

void ParseInputArguments(int32_t argc, char **argv, Proxy::ForwardingData &fwd, Proxy::ModeFunctionPointer* fptr) noexcept
{
    const option longOptions[] = {
            {"mode", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Mode) },
            {"domain", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Domain) },
            {"lp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::ListeningPort) },
            {"dp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::DestinationPort) },
            {"ban", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::BannedDomain) },
    };

    int32_t result;
    int32_t optionIndex;

    while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
    {
        switch (result)
        {
            case static_cast<int32_t>(Proxy::InputArgs::Mode):
            {
                std::cout << "[found: mode]\n";
                switch (strtol(optarg, nullptr,10))
                {
                    case static_cast<int32_t>(Proxy::Mode::Forwarding):
                    {
                        *fptr = (Proxy::ModeFunctionPointer)Proxy::StartForwardingMode;
                        break;
                    }
                    case static_cast<int32_t>(Proxy::Mode::Tracking):
                    {
                        *fptr = (Proxy::ModeFunctionPointer)Proxy::Tracking::StartTrackingMode;
                        break;
                    }
//                    Currenlty struggling with this mode :C. mode(Extension B)
                    case static_cast<int32_t>(Proxy::Mode::Ban):
                    {
                        *fptr = (Proxy::ModeFunctionPointer)Proxy::Ban::StartBanMode;
                        break;
                    }
                    default:
                    {
                        std::cout << "[invalid mode]\n";
                        exit(1);
                    }
                }
                break;
            }
            case static_cast<int32_t>(Proxy::InputArgs::Domain):
            {
                fwd.hostName = std::string(optarg);
                std::cout << "domain: " << fwd.hostName << "\n";
                break;
            }
            case static_cast<int32_t>(Proxy::InputArgs::BannedDomain):
            {
                fwd.bannedHostName = std::string(optarg);
                std::cout << "banned: " << fwd.bannedHostName << "\n";
                break;
            }
            case static_cast<int32_t>(Proxy::InputArgs::ListeningPort):
            {
                fwd.listeningPort = strtol(optarg, nullptr,10);
                std::cout << "listening port: " << fwd.listeningPort << "\n";
                break;
            }
            case static_cast<int32_t>(Proxy::InputArgs::DestinationPort):
            {
                fwd.destinationPort = strtol(optarg, nullptr,10);
                std::cout << "destination port: " << fwd.destinationPort << "\n";
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