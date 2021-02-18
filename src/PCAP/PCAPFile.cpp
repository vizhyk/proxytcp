#include "PCAPFile.hpp"


namespace Proxy::PCAP
{
    PCAPFile::PCAPFile(const std::string& filename) noexcept
    {
        m_pcapfile.open(filename, std::ios_base::binary);
        if(!m_pcapfile)
        {
            exit(static_cast<int32_t>(Status::Error::PCAPFileWasNotCreated));
        }
    }

}


