#include "ServerConnection.h"

namespace Proxy
{
    ServerConnection::ServerConnection(int32_t sockfd, ConnectionState state) noexcept: Connection(sockfd, state) {}
}