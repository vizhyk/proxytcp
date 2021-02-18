#include <iterator>
#include "ByteStream.hpp"

namespace Proxy
{
    ByteStream::ByteStream(const uint8_t* data, std::size_t size) noexcept
        : m_buffer(data,data+size), m_usedBytes(size)
    {}

    ByteStream::ByteStream(size_t bufferSize) noexcept
        : m_buffer(bufferSize), m_usedBytes(0)
    {}

    ByteStream::ByteStream() noexcept
        : m_buffer(512), m_usedBytes(0)
    {}

    ByteStream::operator bool() const noexcept
    {
        return !IsEmpty();
    }

    uint8_t& ByteStream::operator[](std::size_t idx)
    {
        return m_buffer.at(idx);
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

    std::vector<uint8_t>::const_iterator ByteStream::Begin()
    {
        return m_buffer.begin();
    }

    std::vector<uint8_t>::const_iterator ByteStream::End()
    {
        return  m_buffer.end();
    }


    std::size_t ByteStream::GetAvailableBytes() const noexcept
    {
        return m_buffer.size() - m_usedBytes;
    }

    std::size_t ByteStream::GetSize() const noexcept
    {
        return m_buffer.size();
    }

    std::size_t ByteStream::GetUsedBytes() const noexcept
    {
        return m_usedBytes;
    }

    const uint8_t* ByteStream::GetBuffer() const noexcept
    {
        return m_buffer.data();
    }

    bool ByteStream::IsEmpty() const noexcept
    {
        return m_usedBytes == 0;
    }

    bool ByteStream::IsFull() const noexcept
    {
        return m_usedBytes == m_buffer.size();
    }

    void ByteStream::Clear() noexcept
    {
        m_buffer.clear();
        m_usedBytes = 0;
    }

    void ByteStream::Insert( std::istream_iterator<uint8_t> first,  std::istream_iterator<uint8_t> last, std::size_t size) noexcept
    {
        m_buffer.insert(m_buffer.begin(),first,last);
        m_usedBytes += size;
    }

    void ByteStream::Insert(const uint8_t* data, uint32_t dataSize) noexcept
    {
        if(this->GetAvailableBytes() < dataSize)
        {
            Resize((m_buffer.size() + dataSize) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, data, dataSize);
        m_usedBytes += dataSize;
    }

    void ByteStream::Insert(const ByteStream& rhs) noexcept
    {
        auto rhsUsedDataSize = rhs.GetUsedBytes();

        if(this->GetAvailableBytes() < rhsUsedDataSize)
        {
            Resize((m_buffer.size() + rhsUsedDataSize) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, rhs.m_buffer.data(), rhsUsedDataSize);
        m_usedBytes += rhsUsedDataSize;

    }

    void ByteStream::Insert(uint8_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize((m_buffer.size() + valueSizeof) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += sizeof(value);
    }

    void ByteStream::Insert(uint16_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize((m_buffer.size() + valueSizeof) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += sizeof(value);

    }

    void ByteStream::Insert(uint32_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize((m_buffer.size() + valueSizeof) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += sizeof(value);
    }

    void ByteStream::Insert(uint64_t value) noexcept
    {
        auto valueSizeof = sizeof(value);

        if(this->GetAvailableBytes() < valueSizeof)
        {
            Resize((m_buffer.size() + valueSizeof) * 2);
        }

        memcpy(m_buffer.data() + m_usedBytes, &value, valueSizeof);
        m_usedBytes += sizeof(value);
    }



    void ByteStream::Resize(size_t newSize) noexcept
    {
        m_buffer.resize(newSize);
    }

    bool ByteStream::cmp(const ByteStream& rhs) const noexcept
    {
        if(m_usedBytes != rhs.m_usedBytes)
            return false;

        for(std::size_t i = 0; i < m_usedBytes; ++i)
        {
            if(m_buffer[i] != rhs.m_buffer[i])
            {
                return false;
            }
        }

        return true;
    }

    void ByteStream::Erase(std::vector<uint8_t>::const_iterator position)
    {
        m_buffer.erase(position);
        --m_usedBytes;
    }

    void ByteStream::Erase(std::vector<uint8_t>::const_iterator first, std::vector<uint8_t>::const_iterator last, std::size_t numberOfElements)
    {
        m_buffer.erase(first,last);
        m_usedBytes -= numberOfElements;
    }




    bool operator==(const ByteStream& lhs, const ByteStream& rhs) noexcept { return lhs.cmp(rhs); }

    bool operator!=(const ByteStream& lhs, const ByteStream& rhs) noexcept { return !(rhs == lhs); }

}


