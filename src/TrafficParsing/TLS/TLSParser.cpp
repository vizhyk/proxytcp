#include "TLSParser.hpp"
#include "src/Utilities/Constants.hpp"

#include <cstring>
#include <netinet/in.h>
#include <memory>

namespace Proxy::TrafficParsing
{
    using namespace Proxy::Utilities;

    bool TLSParser::IsClientHelloMessage (const uint8_t* buff, int32_t offset) noexcept
    {
        return ( static_cast<uint32_t>(buff[Offsets::TLS::MESSAGE_TYPE - offset]) == 0x16 ) && ( static_cast<uint32_t>(buff[Offsets::TLS::HANDSHAKE_TYPE - offset]) == 0x01 );
    }

    ByteStream TLSParser::GetDomainNameFromTCPPacket (const uint8_t* buffer, uint32_t offset) noexcept
    {
        uint16_t extensionsDataSize = ExtractUint16FromNetworkData(buffer + Offsets::TLS::EXTENTIONS_DATA_SIZE - offset);
        auto extentionsData = std::make_unique<uint8_t[]>(extensionsDataSize);

        memcpy(extentionsData.get(),buffer + Offsets::TLS::EXTENTIONS_DATA - offset, extensionsDataSize);

        ByteStream domainName;
        uint32_t extensionOffset = 0;

        while(extensionOffset < extensionsDataSize)
        {
            const uint16_t extentionType = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset);
            const uint16_t extentionLength = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset + sizeof(extentionType));

            extensionOffset+= sizeof(extentionType) + sizeof(extentionLength);

            if(extentionType == 0)
            {
                extensionOffset += 3;

                const uint16_t domainNameSize = ExtractUint16FromNetworkData(extentionsData.get() + extensionOffset);
                extensionOffset += sizeof(domainNameSize);

                domainName.Insert(extentionsData.get() + extensionOffset,domainNameSize);

                extensionOffset += domainNameSize;

                break;
            }
            else
            {
                extensionOffset += extentionLength;
            }
        }

        return domainName;
    }

    uint16_t TLSParser::ExtractUint16FromNetworkData(const uint8_t* buff) noexcept
    {
        uint16_t tmpValue;
        memcpy(&tmpValue, buff , sizeof(uint16_t));
        tmpValue = ntohs(tmpValue);

        return tmpValue;
    }

    uint32_t TLSParser::ExtractUint32FromNetworkData(const uint8_t* buff) noexcept
    {
        uint32_t tmpValue;
        memcpy(&tmpValue, buff , sizeof(uint32_t));
        tmpValue = ntohl(tmpValue);

        return tmpValue;
    }

}