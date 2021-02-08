#include "ClientConnection.hpp"

namespace Proxy
{
    ClientConnection::ClientConnection(int32_t sockfd, ConnectionState state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept
        : Connection(sockfd, state, pipeline)
    {}
}
