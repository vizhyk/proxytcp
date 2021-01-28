#include "ServerConnection.hpp"

namespace Proxy
{
    ServerConnection::ServerConnection(int32_t sockfd, ConnectionState state, std::shared_ptr<ConversationPipeline> pipeline) noexcept
        : Connection(sockfd, state, pipeline)
    {}
}