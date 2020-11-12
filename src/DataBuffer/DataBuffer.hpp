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
        ~DataBuffer() noexcept = default;

        explicit DataBuffer(size_t bufferSize) noexcept;
        explicit DataBuffer(const char* array) noexcept;

//        DataBuffer(const DataBuffer& rhs) noexcept = default;
//        DataBuffer& operator=(const DataBuffer& rhs) noexcept = default;

        DataBuffer(DataBuffer&& rhs) noexcept = default;
        DataBuffer& operator=(DataBuffer&& rhs) noexcept = default;


        explicit operator bool() const noexcept { return IsEmpty(); }
        char& operator[](std::size_t idx) noexcept { return buffer_[idx]; }

    public:
        uint32_t GetUsedDataSize()   const noexcept { return usedBytes_; }
        uint32_t GetBufferSize()     const noexcept { return bufferSize_; }
        uint32_t GetAvailableBytes() noexcept;
        char*    GetBuffer()         const noexcept { return buffer_.get(); }

        bool IsEmpty() const noexcept { return (bufferSize_ != 0) && (strlen(buffer_.get()) != 0); }

        //TODO: make methods returning status
        void ResizeAndCopyData() noexcept;
        void MarkDataAsRead(int32_t recievedDataSize) noexcept;

    private:
        std::unique_ptr<char[]> buffer_;
        std::size_t bufferSize_;
        std::size_t usedBytes_;
    };

    bool operator==(const Proxy::DataBuffer& lhs, const Proxy::DataBuffer& rhs) noexcept;
    bool operator==(const Proxy::DataBuffer& lhs, const char* rhs)       noexcept;
    bool operator==(const char* lhs, const Proxy::DataBuffer& rhs)       noexcept;

    bool operator!=(const Proxy::DataBuffer& lhs, const Proxy::DataBuffer& rhs) noexcept;
    bool operator!=(const Proxy::DataBuffer& lhs, const char* rhs)       noexcept;
    bool operator!=(const char* lhs, const Proxy::DataBuffer& rhs)       noexcept;

}





#endif //PROXYTCP_DATABUFFER_HPP
