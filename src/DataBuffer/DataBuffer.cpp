#include "DataBuffer.hpp"

namespace Proxy
{
    DataBuffer::DataBuffer(size_t bufferSize) noexcept
        : bufferSize_{bufferSize}, usedBytes_{0}
    {
        //TODO: buffer size + 1
        buffer_ = std::make_unique<char[]>(bufferSize_ + 1);
    }

    DataBuffer::DataBuffer(const char* array) noexcept
        :  bufferSize_{static_cast<uint32_t>(strlen(array))}, usedBytes_{0}
    {
        //TODO: check if strlen is 0
        buffer_ = std::make_unique<char[]>(bufferSize_ + 1);
        memcpy(buffer_.get(), array, bufferSize_);

        usedBytes_ = bufferSize_;
    }

    void DataBuffer::ResizeAndCopyData() noexcept
    {
        auto previousAvailableBytes = bufferSize_;
        bufferSize_ = bufferSize_ * 2;

        auto newBuffer = std::make_unique<char[]>(bufferSize_ + 1);

        memcpy(newBuffer.get(),buffer_.get(),previousAvailableBytes);

        buffer_ = std::move(newBuffer);

    }

    uint32_t DataBuffer::GetAvailableBytes() noexcept
    {
        if(usedBytes_ == 0)
        {
            ResizeAndCopyData();
            return bufferSize_ - usedBytes_;
        }

        return bufferSize_ - usedBytes_;
    }

    void DataBuffer::MarkDataAsRead(int32_t recievedDataSize) noexcept
    {
        //TODO: get error checking
        usedBytes_ += recievedDataSize;

    }




    bool operator==(const Proxy::DataBuffer& lhs, const Proxy::DataBuffer& rhs) noexcept { return strcmp(lhs.GetBuffer(), rhs.GetBuffer()); }
    bool operator==(const Proxy::DataBuffer& lhs, const char* rhs)       noexcept { return strcmp(lhs.GetBuffer(), Proxy::DataBuffer{rhs}.GetBuffer()); }
    bool operator==(const char* lhs, const Proxy::DataBuffer& rhs)       noexcept { return strcmp(Proxy::DataBuffer{lhs}.GetBuffer(), rhs.GetBuffer()); }

    bool operator!=(const Proxy::DataBuffer& lhs, const Proxy::DataBuffer& rhs) noexcept { return !( strcmp(lhs.GetBuffer(), rhs.GetBuffer()) ); }
    bool operator!=(const Proxy::DataBuffer& lhs, const char* rhs)       noexcept { return !( strcmp(lhs.GetBuffer(), Proxy::DataBuffer{rhs}.GetBuffer()) ); }
    bool operator!=(const char* lhs, const Proxy::DataBuffer& rhs)       noexcept { return !( strcmp(Proxy::DataBuffer{lhs}.GetBuffer(), rhs.GetBuffer()) ); }


}

