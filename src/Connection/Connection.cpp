#include "Connection.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket, Connection::Side type) noexcept
        : socket{socket}, type{type} {}

}


