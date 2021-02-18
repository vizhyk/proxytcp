#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
#include "Connection.hpp"
#include <fstream>

namespace Proxy
{

    class SocketCapturingConnection: public Connection
    {
        class PCAPcapturingFile;
    public:
        SocketCapturingConnection(int32_t sockfd, ConnectionState state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;
        Status ReadData() override;
        Status SendData(const ByteStream& data) override;
        void CaptureData(PCAPcapturingFile& file);
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
