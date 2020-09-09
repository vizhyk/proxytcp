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

namespace Proxy
{
    enum class Status : uint8_t
    {
        IncorrectInputArguments = 0xff,
        BadListeningPortConversion = 0xfe,
        BadDestinationPortConversion = 0xfd,
        BadListeningSocketInitializaton = 0xfc,
    };

    struct ForwardingData
    {
        uint16_t listeningPort   {}; // what  to listen
        uint16_t destinationPort {}; // where to forward
        std::string hostName = "coolsite.io";
    };

    void InitForwardingData(int argc, char **argv, ForwardingData &fwd, Status &status) noexcept;
    void TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept;

    int32_t CreateSocketForForwarding(uint16_t destinationPort, const char* hostName) noexcept;
    int32_t CreateSocketOnListeningPort(uint16_t listeningPort, Status &status) noexcept;


}

namespace Proxy::ExtensionA
{

}

namespace Proxy::ExtensionB
{

}