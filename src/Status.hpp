#ifndef PROXYTCP_STATUS_HPP
#define PROXYTCP_STATUS_HPP

#include <cstdint>

namespace Proxy
{
    class Status
    {
    public:
        using CodeType = int32_t;

        enum class Success : CodeType
        {
            Success = 0,
            BannedHostConnectionRefused = 1,
            IgnoredBadConnectionHostDomainName = 2,
            WaitingForData = 3,
            WaitingForConnectResponse = 4,
            WaitingForTLSMessages = 5,
            GeneratedRecordsAndWaitForData = 6,
        };
        enum class Error : CodeType
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
            BadConnectionHostDomainName = -18,
            BadMakingSocketNonblocking = -19,
            BadGetNameInfo = -20,
            BadEpollCreateInstance = - 200000,
            BadEpollWait = - 200001,
            BadEpollCTL = - 200002,
            BadMappingInsertion = -21,
            BadSendingData = -22,
            BadRecievedDataSize = -23,
            BadBufferSize = -24,
            BadGetAddrInfo = -25,
            IncorrectConversationFlowCode = -26,
            ConversationPipelineNotFound = -27,
            NoPipelineFound = -28,
            BadPayloadSize = -29,
            BadConnectionPayload = -30,
            BadGetSockopt = -31,
            BadConnectToServer = -32,
            BadConnectionRequestLenght = -33,
            BadDestinationAddressType = -34,
            NoConversationPipelineFound = -35,
            NoConversationPipelineCreated = -36,
            PacketIsNotFull = -37,
            BadRecordFound = -38,
            BadTLSRecordContentType = -39,
            NoServerHelloFound = -40,
            PCAPFileWasNotCreated = -41,
            ConnectionManagerIsNotInitialized = -42,
        };

    public:
        constexpr Status() noexcept : code_{ 0 } {};
        explicit constexpr Status(int32_t code) noexcept : code_{code} {}
        explicit constexpr Status(Error   code) noexcept : Status{static_cast<int32_t>(code)} {}
        explicit constexpr Status(Success code) noexcept : Status{static_cast<int32_t>(code)} {}
        ~Status() noexcept = default;

        explicit constexpr operator bool() const noexcept { return code_ == static_cast<int32_t>(Success::Success); }

        constexpr Status& operator=(Error code) noexcept { return *this = Status{ code }; }
        constexpr Status& operator=(Success code) noexcept { return *this = Status{ code }; }

        constexpr CodeType Code() const noexcept { return code_; }

        constexpr bool Succeed() const noexcept { return code_ == static_cast<int32_t>(Success::Success); }
        constexpr bool Failed()  const noexcept { return code_ < static_cast<int32_t>(Success::Success); }

    private:
        CodeType code_;
    };

    constexpr bool operator==(Status lhs, Status rhs) noexcept { return lhs.Code() == rhs.Code(); }
    constexpr bool operator==(Status lhs, Status::Error   rhs) noexcept { return lhs == Status { rhs }; }
    constexpr bool operator==(Status lhs, Status::Success rhs) noexcept { return lhs == Status { rhs }; }
    constexpr bool operator==(Status::Error   lhs, Status rhs) noexcept { return Status { lhs } == rhs; }
    constexpr bool operator==(Status::Success lhs, Status rhs) noexcept { return Status { lhs } == rhs; }

    constexpr bool operator!=(Status lhs, Status rhs) noexcept { return !(lhs == rhs); }
    constexpr bool operator!=(Status lhs, Status::Error   rhs) noexcept { return !(lhs == Status { rhs }); }
    constexpr bool operator!=(Status lhs, Status::Success rhs) noexcept { return !(lhs == Status { rhs }); }
    constexpr bool operator!=(Status::Error   lhs, Status rhs) noexcept { return !(Status { lhs } == rhs); }
    constexpr bool operator!=(Status::Success lhs, Status rhs) noexcept { return !(Status { lhs } == rhs); }

} //namespace Proxy




#endif // PROXYTCP_STATUS_HPP