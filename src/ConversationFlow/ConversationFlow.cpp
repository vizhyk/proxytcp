#include "ConversationFlow.hpp"
#include "ConversationPipeline/ConversationPipeline.hpp"
#include "ConversationManager/SocketConversationManager.hpp"
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

    void ConversationFlow::LogData(const std::string& sender, const ByteStream& data) noexcept
    {
        std::cout << "[" << sender << "]: ";
        for(int32_t i = 0; i < data.GetUsedBytes(); ++i)
        {
            printf("%02x ", data.GetBuffer()[i]);
        }
        std::cout << "\n";
    }

}

