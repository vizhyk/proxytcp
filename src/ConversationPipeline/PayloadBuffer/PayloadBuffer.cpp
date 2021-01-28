#include "PayloadBuffer.hpp"

namespace Proxy
{

    ByteStream& PayloadBuffer::data() noexcept
    {
        return *m_data;
    }

//    PayloadBuffer::PayloadBuffer(PayloadBuffer&& rhs) noexcept
//    {
//        m_data = std::move(rhs.m_data);
//    }
//
//    PayloadBuffer& PayloadBuffer::operator=(PayloadBuffer&& rhs) noexcept
//    {
//        m_data = std::move(rhs.m_data);
//        return *this;
//    }
}