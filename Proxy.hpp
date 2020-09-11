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

#include "Status.hpp"

namespace Proxy
{
    struct ForwardingData
    {
        uint16_t listeningPort   {}; // what  to listen
        uint16_t destinationPort {}; // where to forward
        std::string hostName = "coolsite.io";
    };

    Status InitForwardingData(int32_t argc, char **argv, ForwardingData &fwd) noexcept;
    Status CreateSocketForForwarding(int32_t& socketForForwarding, uint16_t destinationPort, const char *hostName) noexcept;
    Status CreateSocketOnListeningPort(int32_t &listeningPort, uint16_t serverPort) noexcept;

    Status TransferData(int32_t sourceSocket, int32_t destinationSocket) noexcept;

    void PrintStatusAndExit(const Status& status) noexcept;
}

namespace Proxy::ExtensionA
{

}

namespace Proxy::ExtensionB
{

}