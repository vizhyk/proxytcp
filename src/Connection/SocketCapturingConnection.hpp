#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP

#include <fstream>
#include "PCAP/PCAPCapturingFile.hpp"
#include "SocketConnection.hpp"

namespace Proxy
{
    class SocketCapturingConnection: public SocketConnection
    {
    public:
        SocketCapturingConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;
        Status ReadData() override;
//        Status SendData(const ByteStream& data) override;
        void CaptureData(PCAP::PCAPCapturingFile& file);
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
