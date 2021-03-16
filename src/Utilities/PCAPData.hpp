#ifndef PROXYTCP_PCAPDATA_HPP
#define PROXYTCP_PCAPDATA_HPP

#include <cstdint>

namespace Proxy
{
    struct PCAPData
    {
        uint32_t m_sequenceNumber;
        uint32_t m_acknowledgmentNumber;
        uint32_t m_IPv4;
        uint16_t port;
    };
}

#endif //PROXYTCP_PCAPDATA_HPP
