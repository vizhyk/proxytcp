#ifndef PROXYTCP_SOCKS5_HPP
#define PROXYTCP_SOCKS5_HPP

#include <cstdint>

namespace Proxy::Utilities
{
    namespace SOCKS5::Handshake
    {
        const uint8_t Version = 0x05;
        const uint8_t RSV = 0x00;
        const uint8_t IPv4 = 0x01;
        const uint8_t IPv6 = 0x04;
        const uint8_t DomainName = 0x03;

        enum  class ATYP
        {
            IPv4 = 0x01,
            IPv6 = 0x04,
            domain = 0x03,
        };


        enum  class ReplyStatus : uint8_t
        {
             Succeeded = 0x00,
             SOCKS_server_failure = 0x01,
             connection_not_allowed = 0x02,
             Network_unreachable = 0x03,
             Host_unreachable = 0x04,
             Connection_refused = 0x05,
             TTL_expired = 0x06,
             Command_not_supported = 0x07,
             Addrtype_not_supported = 0x08,
             XFF_unassign = 0x09,
        };

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

    }
}



#endif //PROXYTCP_SOCKS5_HPP
