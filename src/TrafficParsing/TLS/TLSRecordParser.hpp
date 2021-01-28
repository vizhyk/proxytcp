#ifndef PROXYTCP_TLSRECORDPARSER_HPP
#define PROXYTCP_TLSRECORDPARSER_HPP

#include "src/Status.hpp"

namespace Proxy::TrafficParsing
{
    class TLSRecordParser
    {
    public:
        static uint16_t GetTLSRecordPayloadSize(const uint8_t* buffer, std::size_t bufferSize) noexcept;
        static Status IsRecordFull(const uint8_t* tlsRecordData, uint32_t tlsRecordSize) noexcept;
    };
}

#endif //PROXYTCP_TLSRECORDPARSER_HPP
