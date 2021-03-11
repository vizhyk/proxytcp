#ifndef PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP
#define PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP

#include "Utilities/SYNACKData.hpp"
#include "PCAP/PCAPCapturingFile.hpp"
#include "ConversationPipeline.hpp"

namespace Proxy
{
    class CapturingConversationPipeline : public ConversationPipeline
    {
    public:
        CapturingConversationPipeline(int32_t epollfd, ConversationManager& conversationManager) noexcept;
        CapturingConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, ConversationManager& conversationManager) noexcept;

        void OpenPCAPFile(const std::string& filename) noexcept;

        PCAP::PCAPCapturingFile& PCAPFile() noexcept;

        SYNACKData& ClientSYNACK() noexcept;
        SYNACKData& ServerSYNACK() noexcept;

        void PerformTransaction(int32_t sockfdWithEvent) noexcept override;

        void InitServerConnection(int32_t sockfd) noexcept override;
        void InitClientConnection(int32_t sockfd) noexcept override;

    private:
        std::unique_ptr<PCAP::PCAPCapturingFile> m_pcapfile;
        SYNACKData m_clientSYNACKData { 1, 1 };
        SYNACKData m_serverSYNACKData { 1, 1 };
    };

} //namespace Proxy



#endif //PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP
