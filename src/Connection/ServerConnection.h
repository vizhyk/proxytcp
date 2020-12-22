#ifndef PROXYTCP_SERVERCONNECTION_H
#define PROXYTCP_SERVERCONNECTION_H
#include "Connection.hpp"
namespace Proxy
{
    class ServerConnection : public Connection
    {
    public:

        ServerConnection(int32_t sockfd, ConnectionState state) noexcept;
    };
}

#endif //PROXYTCP_SERVERCONNECTION_H
