#ifndef PROXYTCP_BYTESTREAM_HPP
#define PROXYTCP_BYTESTREAM_HPP

#include "../Status.hpp"

#include <memory>
#include <cstring>



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
        uint8_t& operator[](std::size_t idx) noexcept;

        ByteStream& operator<<(uint8_t value)  noexcept;
        ByteStream& operator<<(uint16_t value) noexcept;
        ByteStream& operator<<(uint32_t value) noexcept;
        ByteStream& operator<<(uint64_t value) noexcept;
        ByteStream& operator<<(const ByteStream& rhs) noexcept;

    public:
        uint8_t*    GetBuffer()         const noexcept;
        std::size_t GetBufferSize()     const noexcept;
        std::size_t GetUsedDataSize()   const noexcept;
        std::size_t GetAvailableBytes() const noexcept;

        bool IsEmpty() const noexcept;
        bool IsFull()  const noexcept;
        bool cmp(const ByteStream& rhs) const noexcept;

        void Reset() noexcept;

        void Insert(uint8_t value)  noexcept;
        void Insert(uint16_t value) noexcept;
        void Insert(uint32_t value) noexcept;
        void Insert(uint64_t value) noexcept;
        void Insert(const ByteStream& data) noexcept;
        void Insert(const uint8_t* data, uint32_t dataSize) noexcept;

        void Resize() noexcept;
        void Resize(size_t newSize) noexcept;

        std::size_t EnsureGetBytesAndResize() noexcept;

        Status UpdateUsedSpace(size_t recievedDataSize) noexcept;

    private:
        std::unique_ptr<uint8_t[]> m_buffer;
        std::size_t m_bufferSize;
        std::size_t m_usedBytes;
    };

    bool operator==(const ByteStream& lhs, const ByteStream& rhs) noexcept;
    bool operator!=(const ByteStream& lhs, const ByteStream& rhs) noexcept;

}





#endif //PROXYTCP_BYTESTREAM_HPP
