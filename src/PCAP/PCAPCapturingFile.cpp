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
}