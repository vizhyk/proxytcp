
#ifndef PROXYTCP_PCAPREPLAYFILE_HPP
#define PROXYTCP_PCAPREPLAYFILE_HPP


#include "PCAPFile.hpp"

namespace Proxy::PCAP
{
    class PCAPReplayFile : public PCAPFile
    {
    public:
        explicit PCAPReplayFile(const std::string& filename) noexcept;

        Status ReadAllDataFromFile() override;

        Status Write(const ByteStream& data) final = 0;
        Status Write(const uint8_t* data, std::size_t dataSize) final = 0;

    public:
        static ByteStream RetrieveNextPacket();

    private:
        std::basic_ifstream<uint8_t> m_pcapfile;
        ByteStream m_pcapFileBuffer;
        std::size_t m_pcapFileBufferOffset;

    };
}


#endif //PROXYTCP_PCAPREPLAYFILE_HPP
