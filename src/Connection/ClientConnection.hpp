#ifndef PROXYTCP_CLIENTCONNECTION_HPP
#define PROXYTCP_CLIENTCONNECTION_HPP

#include "Connection.hpp"


namespace Proxy
{
    class ClientConnection : public Connection
    {
    public:
        ClientConnection(int32_t sockfd, ConnectionState state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;

    };
}

#endif //PROXYTCP_CLIENTCONNECTION_HPP
