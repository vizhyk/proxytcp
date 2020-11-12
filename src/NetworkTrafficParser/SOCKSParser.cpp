#include "SOCKSParser.h"
#include "src/Utilities/SOCKS5.hpp"
#include <cstring>
#include <src/Utilities/Status.hpp>
#include <src/Utilities/Offsets.hpp>
#include <netinet/in.h>

bool SOCKSParser::IsClientInitiationMessage(const char* buffer, int32_t buffersize) {
        if ((sizeof(buffer)/sizeof(char)) == 0)
            return false;
        else
        {
            int32_t methods[buffer[1]];
            uint32_t nMethods = static_cast<uint8_t>(buffer[1]);

            if (buffer[2] == Proxy::Utilities::SOCKS5::Handshake::Version)
            {
                memcpy(&methods[0], &buffer[1], nMethods);
            }
            if(nMethods == 0x00)
                nMethods = 1;
            if(nMethods == 0xFE)
                nMethods = 0xFE - 1;

            for (int i = 1; i < (nMethods + 1); ++i) {
                switch (static_cast<uint8_t>(buffer[1])) {
                    case 0x00:
                    case 0x02:
                    case 0x03:
                    case 0xFE:
                        return true;
                    case 0xFF:
                        return false;

                }

            }
        }
        return false;
    };

    int8_t SOCKSParser::GetClientAuthenticationMethod(const char* buffer, uint32_t bufferSize){
        return (bufferSize > 2) ? buffer[2] : -1;
    };

    int8_t SOCKSParser::GetDestinationAddressType(const char* buffer, uint32_t bufferSize){
        return  buffer[3];
    }

    void SOCKSParser::GetDestinationAddressAndPort(const char* buffer, uint32_t bufferSize, std::string& destinationAddress, uint16_t& port){
        using namespace Proxy::Utilities;

        Status status{};

        char* tmpDestinationAddress {nullptr};
        uint8_t tmpDestinationAddressSize {};

        switch(GetDestinationAddressType(buffer,bufferSize))
        {
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::IPv4):
            {
                tmpDestinationAddressSize = 4;
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::IP_ADDRESS,tmpDestinationAddressSize);

                auto tmpPortOffset = Offsets::SOCKS5::Handshake::Client::IP_ADDRESS + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);
                break;
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::DomainName):
            {
                memcpy(&tmpDestinationAddressSize,buffer + Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME_SIZE, sizeof(tmpDestinationAddressSize));
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME,tmpDestinationAddressSize);

                auto tmpPortOffset =  Offsets::SOCKS5::Handshake::Client::DOMAIN_NAME + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);

                break;
            }
            case static_cast<uint8_t>(SOCKS5::Handshake::AddressType::IPv6):
            {
                tmpDestinationAddressSize = 16;
                tmpDestinationAddress = new char[tmpDestinationAddressSize + 1]; // +1 for C string null-terminator;
                memcpy(tmpDestinationAddress,buffer + Offsets::SOCKS5::Handshake::Client::IP_ADDRESS,tmpDestinationAddressSize);

                auto tmpPortOffset = Offsets::SOCKS5::Handshake::Client::IP_ADDRESS + tmpDestinationAddressSize;

                memcpy(&port,buffer + tmpPortOffset, tmpDestinationAddressSize);
                port = ntohs(port);



                break;
            }

        }
        destinationAddress = std::string(tmpDestinationAddress,tmpDestinationAddressSize);
        delete[] tmpDestinationAddress;
    }


