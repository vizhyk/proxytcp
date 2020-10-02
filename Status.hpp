#ifndef PROXYTCP_STATUS_HPP
#define PROXYTCP_STATUS_HPP

#include <iostream>

namespace Proxy
{
    class Status
    {
    public:
        enum class Success : int32_t
        {
            Success = 0,
            BannedHostConnectionRefused = 1,
        };
        enum class Error : int32_t
        {
            IncorrectInputArguments = -1000000001,
            BadListeningPortConversion = -2,
            BadDestinationPortConversion = -3,
            BadListeningSocketInitializaton = -4,
            BadConnectionFromListeningSocket = -5,
            BadProcessFork = -6,
            BadDestinationHost = -7,
            BadForwardingSocketCreation = -8,
            BadBindSocketToAddress = -9,
            BadBindListeningPortToSocket = -10,
            BadDataWrittenOnForwarding = -11,
            MarkSocketPassive = - 12,
            NoDataReadFromSocket = -13,
            BannedHostDataTransfer = -14,
            BadConnectionSocketToAddress = -15,
            BadRecievingDataFromSocket = -16,
            BadSendingDataToServer = -17,
        };

    public:
        Status() noexcept : code_{0} {};
        explicit Status(int32_t code) noexcept : code_{code} {}
        explicit Status(Error   code) noexcept : Status{static_cast<int32_t>(code)} {}
        explicit Status(Success code) noexcept : Status{static_cast<int32_t>(code)} {}
        ~Status() noexcept = default;

        explicit operator bool() const noexcept { return code_ == static_cast<int32_t>(Success::Success); }

        Status& operator=(Error code) noexcept { return *this = Status{ code }; }
        Status& operator=(Success code) noexcept { return *this = Status{ code }; }

        int32_t Code() const noexcept { return code_; }

        bool Succeed() const noexcept { return code_ == static_cast<int32_t>(Success::Success); }
        bool Failed()  const noexcept { return code_ < static_cast<int32_t>(Success::Success); }

    private:
        int32_t code_;
    };

} //namespace Proxy

#endif // PROXYTCP_STATUS_HPP