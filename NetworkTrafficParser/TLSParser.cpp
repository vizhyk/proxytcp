//
// Created by vizhy on 08.11.20.
//
#include "TLSParser.h"


    bool TLSParser::IsClientHelloMessage  (const char* buff, int32_t offset) noexcept {

    return ( static_cast<uint32_t>(buff[Proxy::Utilities::Offsets::TLS::MESSAGE_TYPE - offset]) == 0x16 ) && ( static_cast<uint32_t>(buff[Proxy::Utilities::Offsets::TLS::HANDSHAKE_TYPE - offset]) == 0x01 );
    }


    std::string TLSParser::GetDomainNameFromTCPPacket (const char* buffer, uint32_t offset) noexcept {

    uint16_t extensionsDataSize = Proxy::ExecutionModes::ExecutionMode::GetUint16FromNetworkData(buffer + Proxy::Utilities::Offsets::TLS::EXTENTIONS_DATA_SIZE - offset);
    char* extentionsData = new char[extensionsDataSize];

    memcpy(extentionsData,buffer + Proxy::Utilities::Offsets::TLS::EXTENTIONS_DATA - offset, extensionsDataSize);

    std::string domainName {};

    uint32_t extensionOffset {};
    while(extensionOffset < extensionsDataSize)
    {
        const uint16_t extenstionType = Proxy::ExecutionModes::ExecutionMode::GetUint16FromNetworkData(extentionsData + extensionOffset);
        const uint16_t extenstionLength = Proxy::ExecutionModes::ExecutionMode::GetUint16FromNetworkData(extentionsData +extensionOffset + sizeof(extenstionType));
        extensionOffset+= sizeof(extenstionType) + sizeof(extenstionLength);

        if(extenstionType == 0) {
            extensionOffset += 3;
            const uint16_t domainNameSize = Proxy::ExecutionModes::ExecutionMode::GetUint16FromNetworkData(extentionsData + extensionOffset) + 1;
            extensionOffset += sizeof(domainNameSize);
            char* tmpDomainName = new char[domainNameSize];
            memcpy(tmpDomainName, extentionsData + extensionOffset, domainNameSize);
            extensionOffset += domainNameSize;
            domainName = tmpDomainName;
            delete [] tmpDomainName;
            break;
        }
        else {
            extensionOffset += extenstionLength;
        }
    }

            delete [] extentionsData;

        return domainName;
    }