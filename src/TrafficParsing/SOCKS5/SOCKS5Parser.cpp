#include "src/Utilities/SOCKS5.hpp"
#include "src/Utilities/Constants.hpp"
#include "SOCKS5Parser.hpp"

#include <netinet/in.h>

namespace Proxy::TrafficParsing
{
    bool SOCKS5Parser::IsValidClientHelloMessage(const uint8_t* buffer, int32_t bufferSize) noexcept
    {
        using namespace Proxy::Utilities;

        if(bufferSize < 3)
        {
            return false;
        }

        if (buffer[0] == SOCKS5::Handshake::Version)
        {
            auto nMethods = static_cast<uint8_t>(buffer[1]);

            if(nMethods <= 0)
            {
                return false;
            }

            for (int i = 2; i < (nMethods + 2); ++i)
            {
                auto method = static_cast<uint8_t>(buffer[i]);
                if (method == static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::NoAuthentication) ||
                    method == static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::GSSAPI) ||
                    method == static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::UsernameAndPassword) ||
                    method <= static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::IanaAssignedLast) ||
                    method <= static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::PrivateMethodsLast))
                {
                    return true;
                }
            }
        }

        return false;
    }

    uint8_t SOCKS5Parser::GetClientAuthenticationMethod(const uint8_t* buffer, uint32_t bufferSize) noexcept
    {
        using namespace Proxy::Utilities;

        return (bufferSize >= 3) ? buffer[2] : static_cast<uint8_t>(SOCKS5::Handshake::AuthenticationMethod::NoAcceptableMethod);
    }

    uint8_t SOCKS5Parser::GetDestinationAddressType(const uint8_t* buffer, uint32_t bufferSize) noexcept
    {
        return (bufferSize >= 4) ? buffer[3] : 0;
    }

    Status
    SOCKS5Parser::GetDestinationAddressAndPort(const uint8_t* buffer, uint32_t bufferSize,ByteStream& destinationAddress, uint16_t& port) noexcept
    {
        using namespace Proxy::Utilities;
        Status status;

        if(bufferSize <= 0)
        {
            status = Status(Status::Error::BadBufferSize);
            return status;
        }
        uint8_t tmpDestinationAddressSize = 0;
        uint8_t connectionRequestHeadingSize = 4; // filed:bytes | VER:1 + CMD:1 + RSV:1 + ATYP:1 |

        switch(GetDestinationAddressType(buffer, bufferSize))
        {
            case static_cast<uint8_t>(SOCKS5::Handshake::IPv4):
            {
                tmpDestinationAddressSize = 4;
                if( bufferSize < (connectionRequestHeadingSize + tmpDestinationAddressSize + sizeof(port)) )
                {
                    status = Status(Status::Error::BadBufferSize);
                    return status;
                }

                MemcpyDestinationAddressAndPortFromBuffer(buffer, tmpDestinationAddressSize, destinationAddress, port, Offsets::SOCKS5::Handshake::Client::IPv4_ADDRESS);
                break;
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::DomainName):
            {
                memcpy(&tmpDestinationAddressSize,buffer + Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME_SIZE, sizeof(tmpDestinationAddressSize));

                if( bufferSize < (connectionRequestHeadingSize + tmpDestinationAddressSize + sizeof(port)) )
                {
                    status = Status(Status::Error::BadBufferSize);
                    return status;
                }

                MemcpyDestinationAddressAndPortFromBuffer(buffer, tmpDestinationAddressSize, destinationAddress, port, Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME);
                break;
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::IPv6):
            {
                tmpDestinationAddressSize = 16;
                if( bufferSize < (connectionRequestHeadingSize + tmpDestinationAddressSize + sizeof(port)) )
                {
                    status = Status(Status::Error::BadBufferSize);
                    return status;
                }

                MemcpyDestinationAddressAndPortFromBuffer(buffer, tmpDestinationAddressSize, destinationAddress, port, Offsets::SOCKS5::Handshake::Client::IPv6_ADDRESS);
                break;
            }

        }

        return status;
    }

    void SOCKS5Parser::MemcpyDestinationAddressAndPortFromBuffer(const uint8_t* buffer, uint8_t destinationAddressSize, ByteStream& stream,
                                                                 uint16_t& port, uint8_t offset) noexcept
    {
        using namespace Proxy::Utilities;

        stream.Insert(buffer+offset, destinationAddressSize);

        auto portOffset = offset + destinationAddressSize;

        memcpy(&port, buffer + portOffset, sizeof(port));
        port = ntohs(port);
    }

    bool SOCKS5Parser::IsValidConnectionRequestMessage(const uint8_t *buffer, int32_t bufferSize) noexcept
    {
        if(bufferSize < 4) { return false; }

        auto atyp = GetDestinationAddressType(buffer, bufferSize);
        if (atyp == 0) { return false; }

        switch(atyp)
        {
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv4):
                if (bufferSize != 4 + 4 + 2) { return false; }
                break;
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::DomainName):
                if(bufferSize < 5) { return false; }
                if (bufferSize != 4 + buffer[4] + 2 + 1) { return false; }
                break;
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv6):
                if (bufferSize != 4 + 16 + 2) { return false; }
                break;
            default: return false;
        }
        return true;
    }

    size_t SOCKS5Parser::GetConnectionRequestLength(const uint8_t *buffer, size_t buffersize)
    {
        if(buffersize < 4) { return -1; }

        auto atyp = GetDestinationAddressType(buffer, buffersize);
        if (atyp == 0) { return -1; }

        switch(atyp)
        {
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv4):
                if (buffersize != 4 + 4 + 2) { return -1; }
                break;
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::DomainName):
                if(buffersize < 5) { return -1; }
                if (buffersize != 4 + buffer[4] + 2 + 1) { return -1; }
                break;
            case static_cast<uint8_t>(Utilities::SOCKS5::Handshake::IPv6):
                if (buffersize != 4 + 16 + 2) { return -1; }
                break;
            default: return -1;
        }
        return buffersize;
    }

}

