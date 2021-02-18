#include "PCAPReplayFile.hpp"

#include <iterator>

namespace Proxy::PCAP
{
    Status PCAPReplayFile::ReadAllDataFromFile(ByteStream& data)
    {
        Status status;

        // Stop eating new lines in binary mode!!!
        m_pcapfile.unsetf(std::ios::skipws);

        // get its size:
        std::size_t fileSize;

        m_pcapfile.seekg(0, std::ios::end);
        fileSize = m_pcapfile.tellg();
        m_pcapfile.seekg(0, std::ios::beg);

        auto first = std::istream_iterator<uint8_t>(m_pcapfile);
        auto last =  std::istream_iterator<uint8_t>();

        std::vector<uint8_t> v;

        v.insert(v.begin(),first,last);

        data.Insert(std::istream_iterator<uint8_t>(m_pcapfile),std::istream_iterator<uint8_t>(m_pcapfile), 0);

        return status;
    }
}


