#include <zconf.h>
#include "Connection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket, ConnectionState state, std::shared_ptr<ConversationPipeline>& pipeline) noexcept
        : m_socket(socket), m_state(state), m_pipeline(pipeline), m_buffer()
    {}

    Connection::~Connection()
    {
        close(m_socket);
    }

    void
    Connection::ChangeState(ConnectionState state) noexcept
    {
        m_state = state;
    }

    void
    Connection::ChangeSockfd(int32_t sockfd) noexcept
    {
        m_socket = sockfd;
    }

    Connection::ConnectionState
    Connection::GetState() const noexcept
    {
        return m_state;
    }

    int32_t
    Connection::GetSocketfd() const noexcept
    {
        return m_socket;
    }

    ByteStream&
    Connection::Buffer() noexcept
    {
        return m_buffer;
    }

    std::shared_ptr<ConversationPipeline>& Connection::Pipeline() noexcept
    {
        return m_pipeline;
    }




    bool operator==(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() == rhs.GetSocketfd(); }
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept { return !(lhs == rhs); }

}

