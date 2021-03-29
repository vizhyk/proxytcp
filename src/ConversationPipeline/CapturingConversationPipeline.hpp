#ifndef PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP
#define PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP

#include "Utilities/PCAPData.hpp"
#include "PCAP/PCAPCapturingFile.hpp"
#include "ConversationPipeline.hpp"

namespace Proxy
{
    class CapturingConversationPipeline : public ConversationPipeline
    {
    public:
        CapturingConversationPipeline(int32_t epollfd, SocketConversationManager& conversationManager) noexcept;
        CapturingConversationPipeline(int32_t epollfd, std::unique_ptr<ConversationFlow> flow, SocketConversationManager& conversationManager) noexcept;

//        ~CapturingConversationPipeline() override
//        {
//            m_pcapfile.Close();
//            std::cout << "file closed\n";
//        }

        PCAP::PCAPCapturingFile& PCAPFile() noexcept;

        PCAPData& ClientPCAPData() noexcept;
        PCAPData& ServerPCAPData() noexcept;

//        Status PerformTransaction(int32_t sockfdWithEvent) noexcept override;

        void InitServerConnection(int32_t sockfd) noexcept override;
        void InitClientConnection(int32_t sockfd) noexcept override;
        void InitPCAPClientData(const PCAPData& pcapData) noexcept;
        void InitPCAPServerData(const PCAPData& pcapData) noexcept;
        void InitPCAPClientData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept;
        void InitPCAPServerData(uint32_t sequenceNumber, uint32_t acknowledgmentNumber, uint32_t IPv4) noexcept;

    private:
        PCAPData m_clientPCAPData;
        PCAPData m_serverPCAPData;
        PCAP::PCAPCapturingFile& m_pcapfile;
    };

} //namespace Proxy



#endif //PROXYTCP_CAPTURINGCONVERSATIONPIPELINE_HPP
