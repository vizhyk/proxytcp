#ifndef PROXYTCP_PAYLOADBUFFER_HPP
#define PROXYTCP_PAYLOADBUFFER_HPP

#include "ByteStream/ByteStream.hpp"
#include <memory>

namespace Proxy
{
    class PayloadBuffer
    {
    public:
        using ByteStreamPtr = std::unique_ptr<ByteStream>;

    public:
        PayloadBuffer() noexcept = default;
//        PayloadBuffer(PayloadBuffer&& rhs) noexcept;
//        PayloadBuffer& operator=(PayloadBuffer&& rhs) noexcept;
        virtual ~PayloadBuffer() noexcept = default;

    public:
        ByteStream& data() noexcept;

    protected:
        ByteStreamPtr m_data;

    };
}



#endif //PROXYTCP_PAYLOADBUFFER_HPP
