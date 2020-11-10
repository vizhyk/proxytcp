#ifndef PROXYTCP_NONCOPYABLE_HPP
#define PROXYTCP_NONCOPYABLE_HPP

namespace Proxy
{
    class NonCopyable
    {
    public:
        NonCopyable() noexcept = default;
        ~NonCopyable() noexcept = default;

        NonCopyable(const NonCopyable&) noexcept = delete;
        NonCopyable& operator=(const NonCopyable&) noexcept = delete;
    };

}


#endif //PROXYTCP_NONCOPYABLE_HPP
