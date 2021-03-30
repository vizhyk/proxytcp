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

    void PCAPCapturingFile::Open(const std::string& filename, std::ios_base::openmode openmode) noexcept
    {
        m_pcapfile.open(filename, openmode);
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

    void PCAPCapturingFile::Close() noexcept
    {
        m_pcapfile.close();
    }
}