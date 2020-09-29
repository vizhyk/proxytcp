#pragma once

#include <cstdio>

#include <cstdlib>
#include <netdb.h>
#include <csignal>

#include <cerrno>
#include <iostream>
#include <cstring>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <unistd.h>

#include "Status.hpp"

namespace Proxy
{
    enum class InputArgs : int32_t
    {
        Mode = 0,
        Domain = 1,
        ListeningPort = 2,
        DestinationPort = 3,
        BannedDomain = 4,
    };

    enum class Mode : int32_t
    {
        Forwarding = 0,
        Tracking = 1,
        Ban = 2,
    };

    struct ForwardingData
    {
        int32_t listeningPort{}; // what  to listen
        int32_t destinationPort{}; // where to forward
        std::string hostName = "coolsite.io";
        std::string bannedHostName = "badsite.io"; // may be changed to map/unordered_map
    };

    // function pointer that used to chose the function that depends on --mode arg.
    typedef void (* ModeFunctionPointer)(const ForwardingData& fwd);

    // socket that will forward data to the destination
    Status CreateSocketForForwarding(int32_t& socketForForwarding, int32_t destinationPort, const char* hostName) noexcept;
    Status CreateSocketOnListeningPort(int32_t& listeningSocket, int32_t listeningPort, sockaddr_in& socketData) noexcept;
    Status TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept;

    void StartForwardingMode(const ForwardingData& fwd) noexcept;
    void PrintStatusAndExit(const Status& status) noexcept;
}

namespace Proxy::Tracking
{
    void StartTrackingMode(const ForwardingData& fwd) noexcept;

    bool IsClientHelloMesasge(const char* buff, int32_t offset) noexcept;

    //string may be replaced with std::string_view due to huge assembly instruction decrease
    // but i'm not sure :)
    std::string GetDomainNameFromTCPPacket(const char* buffer, uint32_t offset) noexcept;
}

namespace Proxy::Ban
{
    void PrintRecievedData(const char* buffer, uint32_t size) noexcept;
    void StartBanMode(const ForwardingData& fwd) noexcept;

    Status TransferDataWithRestriction(int32_t listeningSocket, const std::string& bannedHostname, int32_t listeningPort) noexcept;
}