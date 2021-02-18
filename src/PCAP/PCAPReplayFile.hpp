
#ifndef PROXYTCP_PCAPREPLAYFILE_HPP
#define PROXYTCP_PCAPREPLAYFILE_HPP


#include "PCAPFile.hpp"

namespace Proxy::PCAP
{
    class PCAPReplayFile : public PCAPFile
    {
    public:
        Status ReadAllDataFromFile(ByteStream& data) override;

        Status Write(const ByteStream& data) final = 0;
        Status Write(const uint8_t* data, std::size_t dataSize) final = 0;

    private:

        std::ifstream m_pcapfile;

    };
}


#endif //PROXYTCP_PCAPREPLAYFILE_HPP
