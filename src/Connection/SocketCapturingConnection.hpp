#ifndef PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
#define PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP

#include <fstream>
#include "SocketConnection.hpp"
#include "Utilities/SYNACKData.hpp"
#include "PCAP/PCAPCapturingFile.hpp"

namespace Proxy
{
    class SocketCapturingConnection: public SocketConnection
    {
    public:

    public:
        SocketCapturingConnection(int32_t sockfd, ConnectionSide state, const std::shared_ptr<ConversationPipeline>& pipeline) noexcept;

        Status ReadData() override;
        Status SendDataTo(const ByteStream& data, SocketConnection& recipientConnection) noexcept override;

        static void CaptureData(PCAP::PCAPCapturingFile& file, const ByteStream& data, SYNACKData& senderSYNACKData, SYNACKData& recipientSYNACKData, ConnectionSide senderConnectionSide);
    };
}

#endif //PROXYTCP_SOCKETCAPTURINGCONNECTION_HPP
