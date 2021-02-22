#include "PCAPReplayFile.hpp"

#include <iterator>
#include <fstream>
#include "Utilities/Constants.hpp"

namespace Proxy::PCAP
{
    Status PCAPReplayFile::ReadAllDataFromFile()
    {
        Status status;
        //TODO: add status processing
        m_pcapfile.unsetf(std::ios::skipws);

        m_pcapFileBuffer.Insert(m_pcapfile);

        return status;
    }

    ByteStream PCAPReplayFile::RetrieveNextPacket()
    {
        ByteStream packet;





        return packet;
    }

    PCAPReplayFile::PCAPReplayFile(const std::string& filename) noexcept
        : m_pcapfile(filename), m_pcapFileBufferOffset(HeaderSize::PCAP_GLOBAL_HEADER)
    {

    }
}


