//
// Created by vizhy on 05.11.20.
//

#ifndef PROXYTCP_SOCKSPARSER_H
#define PROXYTCP_SOCKSPARSER_H

#include "src/ExecutionModes/ExecutionMode.hpp"
#include "src/Utilities/SOCKS5.hpp"




    class SOCKSParser {

    public:
        static bool IsClientInitiationMessage(const unsigned char *buffer, int32_t buffersize);
        static int8_t GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize);
        static int8_t GetDestinationAddressType(const char* buffer, uint32_t bufferSize);
        static void GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port);

    };




#endif //PROXYTCP_SOCKSPARSER_H