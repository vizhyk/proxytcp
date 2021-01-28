#ifndef PROXYTCP_SERVERCONNECTION_HPP
#define PROXYTCP_SERVERCONNECTION_HPP

#include "Connection.hpp"

namespace Proxy
{
    class ServerConnection : public Connection
    {
    public:
        ServerConnection(int32_t sockfd, ConnectionState state, std::shared_ptr<ConversationPipeline> pipeline) noexcept;
        ~ServerConnection() override = default;
    };
}

#endif //PROXYTCP_SERVERCONNECTION_HPP
