#include <iostream>
#include "PCAPCapturingFile.hpp"

namespace Proxy::PCAP
{
    Status PCAPCapturingFile::Write(const ByteStream& data)
    {
        Status status;

        m_pcapfile.write(reinterpret_cast<const char*>(data.GetBuffer()), data.GetUsedBytes());

        return status;
    }

    Status PCAPCapturingFile::Write(const uint8_t* data, std::size_t dataSize)
    {
        Status status;

        m_pcapfile.write(reinterpret_cast<const char*>(data), dataSize);

        return status;
    }

    PCAPCapturingFile::PCAPCapturingFile(const std::string& filename) noexcept
    {
        m_pcapfile.open(filename, std::ios_base::binary);
        if(!m_pcapfile.is_open())
        {
            std::cout << "[No pcap file was opened]\n";
            exit(1);
        }
    }

    void PCAPCapturingFile::Open(const std::string& filename) noexcept
    {
        m_pcapfile.open(filename, std::ios_base::binary | std::ios_base::app);
        if(!m_pcapfile.is_open())
        {
            std::cout << "[No pcap file was opened]\n";
            exit(1);
        }
    }

    bool PCAPCapturingFile::IsOpened() noexcept
    {
        return m_pcapfile.is_open();
    }
}