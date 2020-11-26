#include "ClientConnection.h"

namespace Proxy
{
    ClientConnection::ClientConnection(int32_t sockfd) noexcept: Connection(sockfd) {}
}
