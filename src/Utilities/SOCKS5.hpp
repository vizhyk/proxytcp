#ifndef PROXYTCP_SOCKS5_HPP
#define PROXYTCP_SOCKS5_HPP

#include <cstdint>

namespace Proxy::Utilities
{
    namespace SOCKS5::Handshake
    {
        const uint8_t Version = 0x05;

        enum class AuthenticationMethod : uint8_t
        {
            NoAuthentication = 0x00,
            GSSAPI = 0x01,
            UsernameAndPassword = 0x02,
            IanaAssignedFirst   = 0x03,
            IanaAssignedLast    = 0x7F,
            PrivateMethodsFirst = 0x80,
            PrivateMethodsLast  = 0xFE,
            NoAcceptableMethod  = 0xFF,
        };

        enum class Command : uint8_t
        {
            Connect = 0x01,
            Bind    = 0x02,
            UDPAssociate = 0x03,
        };

        enum class AddressType : uint8_t
        {
            IPv4 = 0x01,
            DomainName = 0x03,
            IPv6 = 0x04,
        };
    }
}



#endif //PROXYTCP_SOCKS5_HPP
