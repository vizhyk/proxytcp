#ifndef PROXYTCP_CLIENTCONNECTION_H
#define PROXYTCP_CLIENTCONNECTION_H
#include "Connection.hpp"
namespace Proxy
{
    class ClientConnection : public Proxy::Connection
    {
    public:
        ClientConnection(int32_t sockfd) noexcept;
    };
}

#endif //PROXYTCP_CLIENTCONNECTION_H
