#include "DataBuffer.hpp"

namespace Proxy
{
    DataBuffer::DataBuffer(size_t bufferSize) noexcept
        : bufferSize_{bufferSize}, usedBytes_{bufferSize}
    {
        //TODO: buffer size + 1
        buffer_ = new char[bufferSize_ + 1];
    }

    DataBuffer::DataBuffer(const char* array) noexcept
        :  bufferSize_{static_cast<uint32_t>(strlen(array))}, usedBytes_{bufferSize_}
    {
        //TODO: check if strlen is 0
        buffer_ = new char[bufferSize_ + 1];
        memcpy(buffer_, array, bufferSize_);
    }


    DataBuffer::~DataBuffer() noexcept
    {
        delete[] buffer_;
    }

    void DataBuffer::ResizeAndCopyData() noexcept
    {
        auto previousAvailableBytes = bufferSize_;
        bufferSize_ = bufferSize_ * 2;

        auto newBuffer = new char[bufferSize_ + 1];

        memcpy(newBuffer,buffer_,previousAvailableBytes);

        delete[] buffer_;

        buffer_ = newBuffer;

        newBuffer = nullptr;
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
        MovePointer(recievedDataSize);

    }

    void DataBuffer::MovePointer(int32_t offset) noexcept
    {
        //TODO: get error checking

        if(offset + usedBytes_ < bufferSize_)
        {
            buffer_ += offset;
        }

    }



}

