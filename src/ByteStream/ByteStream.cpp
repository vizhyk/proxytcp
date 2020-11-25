#include "ByteStream.hpp"

namespace Proxy
{
    ByteStream::ByteStream(const uint8_t* data, std::size_t size) noexcept
        : m_bufferSize(size*2), m_usedBytes(size)
    {
        m_buffer = std::make_unique<uint8_t[]>(m_bufferSize);
        memcpy(m_buffer.get(), data, m_usedBytes);

    }

    ByteStream::ByteStream(size_t bufferSize) noexcept
        : m_bufferSize(bufferSize), m_usedBytes(0)
    {
        m_buffer = std::make_unique<uint8_t[]>(m_bufferSize);
    }

    ByteStream::ByteStream() noexcept
            : m_usedBytes(0), m_bufferSize(0), m_buffer(nullptr)
    {}

    ByteStream::operator bool() const noexcept
    {
        return !IsEmpty();
    }

    uint8_t& ByteStream::operator[](std::size_t idx) noexcept
    {
        return m_buffer[idx];
    }

    ByteStream& ByteStream::operator<<(const ByteStream& rhs) noexcept
    {
        Insert(rhs);

        return *this;
    }

    ByteStream& ByteStream::operator<<(uint8_t value)  noexcept
    {
        Insert(value);

        return *this;
    }

    ByteStream& ByteStream::operator<<(uint16_t value) noexcept
    {
        Insert(value);

        return *this;
    }

    ByteStream& ByteStream::operator<<(uint32_t value) noexcept
    {
        Insert(value);

        return *this;
    }

    ByteStream& ByteStream::operator<<(uint64_t value) noexcept
    {
        Insert(value);

        return *this;
    }

    std::size_t ByteStream::GetAvailableBytes() const noexcept
    {
        return m_bufferSize - m_usedBytes;
    }

    std::size_t ByteStream::GetUsedDataSize() const noexcept
    {
        return m_usedBytes;
    }

    std::size_t ByteStream::GetBufferSize() const noexcept
    {
        return m_bufferSize;
    }

    uint8_t* ByteStream::GetBuffer() const noexcept
    {
        return m_buffer.get();
    }

    bool ByteStream::IsEmpty() const noexcept
    {
        return m_bufferSize == 0;
    }

    bool ByteStream::IsFull() const noexcept
    {
        return m_usedBytes == m_bufferSize;
    }

    void ByteStream::Reset() noexcept
    {
        m_buffer.reset(new uint8_t [m_bufferSize]);
        m_usedBytes = 0;
    }

    void ByteStream::Insert(const uint8_t* data, uint32_t dataSize) noexcept
    {
        if(this->GetAvailableBytes() < dataSize)
        {
            Resize(this->GetUsedDataSize() + dataSize);
        }

        memcpy(m_buffer.get() + m_usedBytes, data, dataSize);
        m_usedBytes += dataSize;
    }

    void ByteStream::Insert(const ByteStream& data) noexcept
    {
        auto rhsUsedDataSize = data.GetUsedDataSize();

        if(this->GetAvailableBytes() < rhsUsedDataSize)
        {
            Resize(this->GetUsedDataSize() + rhsUsedDataSize);
        }

        memcpy(m_buffer.get() + m_usedBytes, data.GetBuffer(), rhsUsedDataSize);
        m_usedBytes += rhsUsedDataSize;
    }

    void ByteStream::Insert(uint8_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize(this->GetUsedDataSize() + valueSizeof);
        }

        memcpy(m_buffer.get() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += valueSizeof;
    }

    void ByteStream::Insert(uint16_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize(this->GetUsedDataSize() + valueSizeof);
        }

        memcpy(m_buffer.get() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += valueSizeof;
    }

    void ByteStream::Insert(uint32_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize(this->GetUsedDataSize() + valueSizeof);
        }

        memcpy(m_buffer.get() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += valueSizeof;
    }

    void ByteStream::Insert(uint64_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize(this->GetUsedDataSize() + valueSizeof);
        }

        memcpy(m_buffer.get() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += valueSizeof;
    }

    void ByteStream::Resize() noexcept
    {
        auto previousAvailableBytes = m_bufferSize;
        m_bufferSize = m_bufferSize * 2;

        auto newBuffer = std::make_unique<uint8_t []>(m_bufferSize);

        memcpy(newBuffer.get(), m_buffer.get(), previousAvailableBytes);

        m_buffer = std::move(newBuffer);
    }

    void ByteStream::Resize(size_t newSize) noexcept
    {
        auto previousAvailableBytes = m_bufferSize;
        m_bufferSize = newSize;

        auto newBuffer = std::make_unique<uint8_t []>(m_bufferSize);

        memcpy(newBuffer.get(), m_buffer.get(), previousAvailableBytes);

        m_buffer = std::move(newBuffer);
    }

    bool ByteStream::cmp(const ByteStream& rhs) const noexcept
    {
        if(m_usedBytes != rhs.m_usedBytes)
            return false;

        for(std::size_t i = 0; i < m_usedBytes; ++i)
        {
            if(m_buffer[i] != rhs.m_buffer[i])
                return false;
        }

        return true;

    }

    Status ByteStream::UpdateUsedSpace(size_t recievedDataSize) noexcept
    {
        Status status;
        if(recievedDataSize < 0)
        {
            status = Status(Status::Error::BadRecievedDataSize);
        }
        m_usedBytes += recievedDataSize;

        return status;

    }

    std::size_t ByteStream::EnsureGetBytesAndResize() noexcept
    {
        if (IsFull())
        {
            Resize();
            return GetAvailableBytes();
        }
        return GetAvailableBytes();
    }


    bool operator==(const ByteStream& lhs, const ByteStream& rhs) noexcept { return lhs.cmp(rhs); }

    bool operator!=(const ByteStream& lhs, const ByteStream& rhs) noexcept { return !( lhs.cmp(rhs) ); }

}


