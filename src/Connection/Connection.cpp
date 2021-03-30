#include <zconf.h>
#include "Connection.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"

namespace Proxy
{
    Connection::Connection(int32_t socket, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept
        : m_socket(socket), m_connectionSide(state), m_buffer(), m_pipeline(pipeline)
    {}

    Connection::~Connection()
    {
        close(m_socket);
    }

    Connection::ConnectionSide
    Connection::GetConnectionSide() const noexcept
    {
        return m_connectionSide;
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

    std::weak_ptr<ConversationPipeline>& Connection::Pipeline() noexcept
    {
        return m_pipeline;
    }


    bool operator==(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() == rhs.GetSocketfd(); }
    bool operator!=(const Connection& lhs, const Connection& rhs) noexcept { return lhs.GetSocketfd() != rhs.GetSocketfd(); }

}

