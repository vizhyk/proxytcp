#ifndef PROXYTCP_OFFSETS_HPP
#define PROXYTCP_OFFSETS_HPP

#include <cstdint>

namespace Proxy::Utilities::Offsets
{
    namespace TLS
    {
        const uint32_t TLS_DATA = 66; // start point of the tls data;
        const uint32_t MESSAGE_TYPE = 66;
        const uint32_t HANDSHAKE_TYPE = 71;
        const uint32_t EXTENTIONS_DATA_SIZE = 178;
        const uint32_t EXTENTIONS_DATA = 180;
    };

    namespace TCP
    {
        const uint32_t TCP_DATA = 34;
    };

    namespace IP
    {
        const uint32_t IP_DATA = 14;
    };

    namespace ETH
    {

    };

    namespace SOCKS5
    {
        namespace Handshake
        {
            namespace Client
            {
                const uint8_t IP_ADDRESS = 4;
                const uint8_t DOMAIN_NAME = 5;
                const uint8_t DOMAIN_NAME_SIZE = 4;
            }

            namespace Server
            {

            }

        }
    };

} // namespace Proxy::Offsets



#endif //PROXYTCP_OFFSETS_HPP
