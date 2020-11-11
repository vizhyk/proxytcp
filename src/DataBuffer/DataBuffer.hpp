#ifndef PROXYTCP_DATABUFFER_HPP
#define PROXYTCP_DATABUFFER_HPP

#include <memory>
#include <cstring>

namespace Proxy
{
    class DataBuffer
    {
    public:
        DataBuffer() noexcept = default;
        ~DataBuffer() noexcept;

        explicit DataBuffer(size_t bufferSize) noexcept;
        explicit DataBuffer(const char* array) noexcept;

        explicit operator bool() const noexcept { return IsEmpty(); }
        char& operator[](std::size_t idx) noexcept { return buffer_[idx]; }

    public:
        uint32_t GetUsedDataSize()   const noexcept { return usedBytes_; }
        uint32_t GetBufferSize()     const noexcept { return bufferSize_; }
        uint32_t GetAvailableBytes() noexcept;
        char*    GetBuffer()         const noexcept { return buffer_; }

        bool IsEmpty() const noexcept { return (bufferSize_ != 0) && (strlen(buffer_) != 0); }

        //TODO: make methods returning status
        void ResizeAndCopyData() noexcept;
        void MarkDataAsRead(int32_t recievedDataSize) noexcept;
        void MovePointer(int32_t offset) noexcept;


    private:
        char*    buffer_;
        std::size_t bufferSize_;
        std::size_t usedBytes_;
    };

    bool operator==(const DataBuffer& lhs, const DataBuffer& rhs) noexcept { return strcmp(lhs.GetBuffer(), rhs.GetBuffer()); }
    bool operator==(const DataBuffer& lhs, const char* rhs)       noexcept { return strcmp(lhs.GetBuffer(), DataBuffer{rhs}.GetBuffer()); }
    bool operator==(const char* lhs, const DataBuffer& rhs)       noexcept { return strcmp(DataBuffer{lhs}.GetBuffer(), rhs.GetBuffer()); }

    bool operator!=(const DataBuffer& lhs, const DataBuffer& rhs) noexcept { return !( strcmp(lhs.GetBuffer(), rhs.GetBuffer()) ); }
    bool operator!=(const DataBuffer& lhs, const char* rhs)       noexcept { return !( strcmp(lhs.GetBuffer(), DataBuffer{rhs}.GetBuffer()) ); }
    bool operator!=(const char* lhs, const DataBuffer& rhs)       noexcept { return !( strcmp(DataBuffer{lhs}.GetBuffer(), rhs.GetBuffer()) ); }

}


#endif //PROXYTCP_DATABUFFER_HPP
