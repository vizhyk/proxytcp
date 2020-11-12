#ifndef PROXYTCP_SOCKSPARSER_H
#define PROXYTCP_SOCKSPARSER_H

//#include "src/ExecutionModes/ExecutionMode.hpp"

#include <string>
#include <cstdint>


    class SOCKSParser {

    public:
        static bool IsClientInitiationMessage(const char* buffer, int32_t buffersize);
        static int8_t GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize);
        static int8_t GetDestinationAddressType(const char* buffer, uint32_t bufferSize);
        static void GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port);

    };




#endif //PROXYTCP_SOCKSPARSER_H