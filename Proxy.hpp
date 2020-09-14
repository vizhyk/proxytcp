#pragma once

#include <cstdio>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <netdb.h>
#include <csignal>
#include <zconf.h>
#include <cerrno>
#include <iostream>
#include <getopt.h>
#include<netinet/tcp.h>
#include<net/ethernet.h>

#include "Status.hpp"

namespace Proxy
{
    enum class InputArgs : int32_t
    {
        Mode = 0,
        Domain = 1,
        ListeningPort = 2,
        DestinationPort = 3,
    };

    enum class Mode : int32_t
    {
        Forwarding = 0,
        Tracking = 1,
        Ban = 2,
    };

    struct ForwardingData
    {
        int32_t listeningPort   {}; // what  to listen
        int32_t destinationPort {}; // where to forward
        std::string hostName = "coolsite.io";
    };

    typedef void (*FunctionPointer)(const ForwardingData& fwd);

    void StartForwardingMode(const ForwardingData &fwd) noexcept;


    Status CreateSocketForForwarding(int32_t& socketForForwarding, int32_t destinationPort, const char *hostName) noexcept;
    Status CreateSocketOnListeningPort(int32_t &listeningSocket, int32_t listeningPort, sockaddr_in &socketData) noexcept;
    Status TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept;

    void PrintStatusAndExit(const Status& status) noexcept;
}

namespace Proxy::Tracking
{
    void StartTrackingMode(const ForwardingData &fwd) noexcept;

    bool IsClientHelloMesasge(const char* buff) noexcept;

    //string may be replaced with std::string_view due to huge assembly instruction decrease
    // but i'm not sure :)
    std::string GetDomainNameFromTCPPacket(const char* buffer) noexcept;
}

namespace Proxy::Ban
{
    void StartBanMode(const ForwardingData &fwd) noexcept;
}