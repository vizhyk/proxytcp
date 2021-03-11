#include "ConversationFlow.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/ConversationManager.hpp"
#include "Connection/SocketConnection.hpp"
#include "Connection/SocketCapturingConnection.hpp"

namespace Proxy
{
    Status
    ConversationFlow::ReadAllDataFromConnection(SocketConnection& connection) noexcept
    {
        return connection.ReadData();
    }

    Status
    ConversationFlow::SendAllDataFromConnection(const ByteStream& data, SocketConnection& connection) noexcept
    {
        return connection.SendData(data);
    }

    Status ConversationFlow::SendAllDataToConnection(const ByteStream& data, SocketConnection& recipient) noexcept
    {
        return recipient.SendDataTo(data,recipient);
    }

}

