#include "ClientConnection.h"

namespace Proxy
{
    ClientConnection::ClientConnection(int32_t sockfd, ConnectionState state) noexcept: Connection(sockfd, state) {}
}
