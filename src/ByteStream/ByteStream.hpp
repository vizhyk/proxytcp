#ifndef PROXYTCP_BYTESTREAM_HPP
#define PROXYTCP_BYTESTREAM_HPP

#include "../Status.hpp"

#include <vector>
#include <cstring>
#include <iterator>

namespace Proxy
{
    class ByteStream
    {
    public:

        ByteStream() noexcept;
        ~ByteStream() noexcept = default;

        ByteStream(const uint8_t* data, std::size_t size) noexcept;
        explicit ByteStream(size_t bufferSize) noexcept;

        ByteStream(ByteStream&& rhs) noexcept = default;
        ByteStream& operator=(ByteStream&& rhs) noexcept = default;

        explicit operator bool() const noexcept;
        uint8_t& operator[](std::size_t idx);
        const uint8_t& operator[](std::size_t idx) const;

        ByteStream& operator<<(uint8_t value)  noexcept;
        ByteStream& operator<<(uint16_t value) noexcept;
        ByteStream& operator<<(uint32_t value) noexcept;
        ByteStream& operator<<(uint64_t value) noexcept;
        ByteStream& operator<<(const ByteStream& rhs) noexcept;

    public:
        std::vector<uint8_t>::const_iterator Begin();
        std::vector<uint8_t>::const_iterator End();

        const uint8_t* GetBuffer()      const noexcept;
        std::size_t GetUsedBytes()      const noexcept;
        std::size_t GetSize()           const noexcept;
        std::size_t GetAvailableBytes() const noexcept;

        bool IsEmpty() const noexcept;
        bool IsFull()  const noexcept;
        bool cmp(const ByteStream& rhs) const noexcept;

        void Clear() noexcept;

        void Insert(uint8_t value)  noexcept;
        void Insert(uint16_t value) noexcept;
        void Insert(uint32_t value) noexcept;
        void Insert(uint64_t value) noexcept;
        void Insert(const ByteStream& rhs) noexcept;
        void Insert(std::basic_istream<uint8_t>& is) noexcept;
        void Insert(const uint8_t* data, uint32_t dataSize) noexcept;

        void Erase(std::vector<uint8_t>::const_iterator position);
        void Erase(std::vector<uint8_t>::const_iterator first, std::vector<uint8_t>::const_iterator last, std::size_t numberOfElements);

        void Resize(size_t newSize) noexcept;

    private:
        std::vector<uint8_t> m_buffer;
        std::size_t m_usedBytes;
    };

    bool operator==(const ByteStream& lhs, const ByteStream& rhs) noexcept;
    bool operator!=(const ByteStream& lhs, const ByteStream& rhs) noexcept;

}





#endif //PROXYTCP_BYTESTREAM_HPP
