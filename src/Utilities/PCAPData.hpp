#ifndef PROXYTCP_PCAPDATA_HPP
#define PROXYTCP_PCAPDATA_HPP

#include <cstdint>

namespace Proxy
{
    struct timestamp
    {
        //timestamp::value
        uint32_t TSval;

        //timestamp::echo reply
        uint32_t TSecr;
    };

    struct PCAPData
    {
        uint32_t m_sequenceNumber;
        uint32_t m_acknowledgmentNumber;
        uint32_t m_ipv4;
        uint16_t m_port;
        timestamp m_timestamp;
    };

    struct PCAPTimestamp
    {
        uint32_t TSsec;
        uint32_t TSusec;
    };
}

#endif //PROXYTCP_PCAPDATA_HPP
