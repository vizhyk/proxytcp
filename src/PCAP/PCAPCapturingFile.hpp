#ifndef PROXYTCP_PCAPCAPTURINGFILE_HPP
#define PROXYTCP_PCAPCAPTURINGFILE_HPP

#include "PCAPFile.hpp"

#include <iostream>

namespace Proxy::PCAP
{
    class PCAPCapturingFile : public PCAPFile
    {
    public:
        PCAPCapturingFile() = default;
        ~PCAPCapturingFile() noexcept
        {
            m_pcapfile.close();
            std::cout << "file closed\n";
        }
        explicit PCAPCapturingFile(const std::string& filename) noexcept;

        void Open(const std::string& filename, std::ios_base::openmode openmode) noexcept;
        void Close() noexcept;
        void Flush() { m_pcapfile.flush(); }
        bool IsOpened() noexcept;

        void seekp(std::_Ios_Seekdir seekdir) noexcept;

        Status ReadAllDataFromFile() override { return Status(Status::Error::NoDataReadFromSocket); };

        Status Write(const ByteStream& data) override;
        Status Write(const uint8_t* data, std::size_t dataSize) override;

    private:
        std::ofstream m_pcapfile;

    };
}
#endif //PROXYTCP_PCAPCAPTURINGFILE_HPP
