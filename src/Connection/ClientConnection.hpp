#ifndef PROXYTCP_CLIENTCONNECTION_HPP
#define PROXYTCP_CLIENTCONNECTION_HPP

#include "Connection.hpp"


namespace Proxy
{
    class ClientConnection : public Connection
    {
    public:
        ClientConnection(int32_t sockfd, ConnectionState state, std::shared_ptr<ConversationPipeline> pipeline) noexcept;
        ~ClientConnection() override = default;

    };
}

#endif //PROXYTCP_CLIENTCONNECTION_HPP
