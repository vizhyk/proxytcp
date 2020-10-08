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
        const uint32_t SNI_SIZE = 192;
        const uint32_t SNI = 193;

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

} // namespace Proxy::Offsets



#endif //PROXYTCP_OFFSETS_HPP
