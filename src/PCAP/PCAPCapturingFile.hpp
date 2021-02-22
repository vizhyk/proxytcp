#ifndef PROXYTCP_PCAPCAPTURINGFILE_HPP
#define PROXYTCP_PCAPCAPTURINGFILE_HPP

#include "PCAPFile.hpp"

namespace Proxy::PCAP
{
    class PCAPCapturingFile : public PCAPFile
    {
    public:
        Status ReadAllDataFromFile() final = 0;

        Status Write(const ByteStream& data) override;
        Status Write(const uint8_t* data, std::size_t dataSize) override;

    private:
        std::ofstream m_pcapfile;

    };
}
#endif //PROXYTCP_PCAPCAPTURINGFILE_HPP
