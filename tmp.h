// mandatory

// accept -> new Connection
// new Connection push to sockfd_set
//
#include "src/Utilities/Status.hpp"

class Connection
{

public:

    // type
    // state
    // sockfd
};

namespace Proxy
{
    class SOCK5Parser
    {
    public:
        // socks : 2
        // tls :
        static bool IsClientInitiationMessage(buff, buffSize);

        static uint8_t GetClientAuthenticationMethod(buff, buffSize)
        static std::string GetDestinationAddress(buff, buffSize)
        static uint16_t GetDestinationPort(buff, buffSize)
    };
    bool tmp = SOCK5Parser::IsClientInitiationMessage();

    class TLSParser
    {

    };
}






