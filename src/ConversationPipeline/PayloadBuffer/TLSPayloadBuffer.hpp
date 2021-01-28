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
            int32_t dataToRead;
            int32_t recordSize;
            int32_t messageSize;
            bool waitingForMoreData;
        };

    public:
        TLSPayloadBuffer() noexcept;
//        explicit TLSPayloadBuffer(PayloadBuffer& payloadBuffer) noexcept;

        TLSOptionalData& Optdata() noexcept;

    private:
        TLSOptionalData optdata;
    };
}




#endif //PROXYTCP_TLSPAYLOADBUFFER_HPP
