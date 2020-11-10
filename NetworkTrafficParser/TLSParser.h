//
// Created by vizhy on 08.11.20.
//

#ifndef PROXYTCP_TLSPARSER_H
#define PROXYTCP_TLSPARSER_H

#include "src/ExecutionModes/ExecutionMode.hpp"
#include "src/Utilities/SOCKS5.hpp"


    class TLSParser {


    public:
        static std::string GetDomainNameFromTCPPacket(const char* buffer, uint32_t offset = 0) noexcept;
        static bool IsClientHelloMessage(const char* buff, int32_t offset = 0) noexcept;

    };



#endif //PROXYTCP_TLSPARSER_H
