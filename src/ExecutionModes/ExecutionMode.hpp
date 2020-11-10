#ifndef PROXYTCP_EXECUTIONMODE_HPP
#define PROXYTCP_EXECUTIONMODE_HPP

#include "src/ConnectionInfo/ConnectionInfo.hpp"
#include "src/Utilities/Ports.hpp"
#include "src/Utilities/Status.hpp"
#include "src/Utilities/Offsets.hpp"
#include "src/ThreadManager/ThreadPool.hpp"

#include <iostream>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>


namespace Proxy::ExecutionModes
{
    enum class ExecutionModeType: uint32_t
    {
        Forwarding = 0,
        Tracking = 1,
        Ban = 2,
        SOCKS5 = 5,
    };

    class ExecutionMode
    {

    public:
        virtual ~ExecutionMode() = default;
        virtual int32_t Run(const ConnectionInfo& info, ThreadPool<std::function<void()>> & threadPool) const noexcept = 0;

        static Utilities::Status CreateForwardingSocketByHostname(int32_t& socketForForwarding, int32_t destinationPort, const std::string& hostName) noexcept;
        static Utilities::Status CreateForwardingSocketByIP(int32_t& socketForForwarding, int32_t destinationPort, const std::string& addr) noexcept;
        static Utilities::Status CreateSocketOnListeningPort(int32_t& listeningSocket, int32_t listeningPort, sockaddr_in& socketData) noexcept;
        Utilities::Status TransferData(int32_t listeningSocket, int32_t destinationSocket) const noexcept;
        Utilities::Status TransferDataWithRestriction(int32_t listeningSocket, const std::string& bannedHostname, int32_t destinationPort) const noexcept;

        static uint16_t GetUint16FromNetworkData(const char* buff) noexcept;
        static uint32_t GetUint32FromNetworkData(const char* buff) noexcept;

        static std::string GetDomainNameFromTCPPacket(const char* buffer, uint32_t offset = 0) noexcept;

        static bool IsClientHelloMesasge(const char* buff, int32_t offset = 0) noexcept;

        static void PrintStatus(const Utilities::Status& status) noexcept;
        void PrintStatusAndTerminateProcess(const Utilities::Status& status) const noexcept;
        static void PrintNetworkData(const char* buffer, uint32_t size) noexcept;
    };

} //namespace Proxy::ExecutionModes




#endif //PROXYTCP_EXECUTIONMODE_HPP
