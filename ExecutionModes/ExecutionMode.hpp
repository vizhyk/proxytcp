#ifndef PROXYTCP_EXECUTIONMODE_HPP
#define PROXYTCP_EXECUTIONMODE_HPP

#include "ConnectionInfo/ConnectionInfo.hpp"
#include "Utilities/Ports.hpp"
#include "Utilities/Status.hpp"
#include "Utilities/Offsets.hpp"

#include <iostream>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <thread>

namespace Proxy::ExecutionModes
{
    enum class ExecutionModeType: uint32_t
    {
        Forwarding = 0,
        Tracking = 1,
        Ban = 2,
    };

    class ExecutionMode
    {

    public:
        virtual ~ExecutionMode() = default;
        virtual Utilities::Status Run(const ConnectionInfo& info) const noexcept = 0;

        Utilities::Status CreateSocketForForwarding(int32_t& socketForForwarding, int32_t destinationPort, const char* hostName) const noexcept;
        Utilities::Status CreateSocketOnListeningPort(int32_t& listeningSocket, int32_t listeningPort, sockaddr_in& socketData) const noexcept;
        Utilities::Status TransferData(int32_t sourceSocket, int32_t destinationSocket) const noexcept;
        Utilities::Status TransferDataWithRestriction(int32_t listeningSocket, const std::string& bannedHostname, int32_t destinationPort) const noexcept;

        std::string GetDomainNameFromTCPPacket(const char* buffer, uint32_t offset = 0) const noexcept;

        bool IsClientHelloMesasge(const char* buff, int32_t offset = 0) const noexcept;

        void PrintStatusAndTerminateProcess(const Utilities::Status& status) const noexcept;
        void PrintRecievedData(const char* buffer, uint32_t size) const noexcept;

    };

} //namespace Proxy::ExecutionModes




#endif //PROXYTCP_EXECUTIONMODE_HPP
