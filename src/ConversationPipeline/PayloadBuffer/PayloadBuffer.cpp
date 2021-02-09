#include "PayloadBuffer.hpp"

namespace Proxy
{

    ByteStream& PayloadBuffer::data() noexcept
    {
        return *m_data;
    }

}