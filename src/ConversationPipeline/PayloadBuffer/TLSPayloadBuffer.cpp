#include "TLSPayloadBuffer.hpp"

namespace Proxy
{
    TLSPayloadBuffer::TLSPayloadBuffer() noexcept
        : optdata{0, false}
    {
        m_data = std::make_unique<ByteStream>();
    }

//    TLSPayloadBuffer::TLSPayloadBuffer(PayloadBuffer& payloadBuffer) noexcept
//        : PayloadBuffer(std::move(payloadBuffer)), optdata{0,0,0}
//    {
//
//    }

    TLSPayloadBuffer::TLSOptionalData& TLSPayloadBuffer::Optdata() noexcept
    {
        return optdata;
    }
}