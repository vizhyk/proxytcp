#ifndef PROXYTCP_SYNACKDATA_HPP
#define PROXYTCP_SYNACKDATA_HPP

#include <cstdint>

namespace Proxy
{
    struct SYNACKData
    {
        uint32_t m_sequenceNumber;
        uint32_t m_acknowledgmentNumber;
    };
}

#endif //PROXYTCP_SYNACKDATA_HPP
