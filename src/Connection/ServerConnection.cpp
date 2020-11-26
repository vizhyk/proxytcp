#include "ServerConnection.h"

namespace Proxy
{
    ServerConnection::ServerConnection(int32_t sockfd) noexcept: Connection(sockfd) {}
}