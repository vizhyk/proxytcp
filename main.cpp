#include "Proxy.hpp"

// usage
// ./proxy [port:forward_from] [port:forward_to]
void ParseInputArguments(int32_t argc, char **argv, Proxy::ForwardingData &fwd, Proxy::FunctionPointer* fptr) noexcept;

int main(int argc, char** argv)
{
    Proxy::ForwardingData fwd {};

    // it holds address of a function that depends on mode choosen by user.[forwarding/tracking(extension A)/Ban(Extension B)
    Proxy::FunctionPointer fptr = nullptr;

    ParseInputArguments(argc,argv,fwd,&fptr);

    printf("address of function in main fptr() is :%p\n", fptr);
    fptr(fwd);

    return 0;
}

void ParseInputArguments(int32_t argc, char **argv, Proxy::ForwardingData &fwd, Proxy::FunctionPointer* fptr) noexcept
{
    const option longOptions[] = {
            {"mode", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Mode) },
            {"domain", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::Domain) },
            {"lp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::ListeningPort) },
            {"dp", required_argument, nullptr , static_cast<int32_t>(Proxy::InputArgs::DestinationPort) },
    };

    int32_t result;
    int32_t optionIndex;

    while ((result = getopt_long(argc, argv, "", longOptions, &optionIndex)) != -1)
    {
        switch (result)
        {
            case static_cast<int32_t>(Proxy::InputArgs::Mode):
            {
                std::cout << "found: mode\n";
                switch (strtol(optarg, nullptr,10))
                {
                    case static_cast<int32_t>(Proxy::Mode::Forwarding):
                    {
                        *fptr = (Proxy::FunctionPointer)Proxy::StartForwardingMode;
                        break;
                    }
                    case static_cast<int32_t>(Proxy::Mode::Tracking):
                    {
                        std::cout << "tracking mode\n";
                        *fptr = (Proxy::FunctionPointer)Proxy::Tracking::StartTrackingMode;
                        break;
                    }
                    case static_cast<int32_t>(Proxy::Mode::Ban):
                    {
                        std::cout << "ban mode\n";
                        *fptr = (Proxy::FunctionPointer)Proxy::Ban::StartBanMode;
                        break;
                    }
                    default:
                    {
                        std::cout << "invalid mode\n";
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


    return status;
}