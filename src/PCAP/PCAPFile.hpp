#ifndef PROXYTCP_PCAPFILE_HPP
#define PROXYTCP_PCAPFILE_HPP

#include <string>
#include <fstream>
#include <ByteStream/ByteStream.hpp>
#include "Status.hpp"

namespace Proxy::PCAP
{
    class PCAPFile
    {
    public:
        PCAPFile() = default;

        PCAPFile(const PCAPFile& pcap) noexcept = delete;
        PCAPFile& operator=(const PCAPFile& pcap) noexcept = delete;

    public:
        virtual Status ReadAllDataFromFile() = 0;
        virtual Status Write(const ByteStream& data) = 0;
        virtual Status Write(const uint8_t* data, std::size_t dataSize) = 0;

    };
}



#endif //PROXYTCP_PCAPFILE_HPP
