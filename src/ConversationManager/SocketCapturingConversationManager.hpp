#ifndef PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP
#define PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP

#include "PCAP/PCAPCapturingFile.hpp"
#include "SocketConversationManager.hpp"

namespace Proxy
{
    class SocketCapturingConversationManager : public SocketConversationManager
    {
    public:
        std::shared_ptr<ConversationPipeline> AddNewPipeline(int32_t clientSockfd, int32_t epollfd) override;

        void OpenPCAPFile(const std::string& filePath) noexcept;

        PCAP::PCAPCapturingFile& PCAPCapturingFile() noexcept;
    private:
        PCAP::PCAPCapturingFile m_pcapfile;

    };
}




#endif //PROXYTCP_SOCKETCAPTURINGCONVERSATIONMANAGER_HPP
