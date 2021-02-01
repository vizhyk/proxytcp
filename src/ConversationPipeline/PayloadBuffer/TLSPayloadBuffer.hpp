#ifndef PROXYTCP_TLSPAYLOADBUFFER_HPP
#define PROXYTCP_TLSPAYLOADBUFFER_HPP

#include "PayloadBuffer.hpp"

namespace Proxy
{
    class TLSPayloadBuffer : public PayloadBuffer
    {

    public:
        struct TLSOptionalData
        {
            std::size_t expectedBytesInTheNextPacket;
            bool waitingForData;
        };

    public:
        TLSPayloadBuffer() noexcept;

        TLSOptionalData& Optdata() noexcept;

    private:
        TLSOptionalData optdata;
    };
}




#endif //PROXYTCP_TLSPAYLOADBUFFER_HPP
