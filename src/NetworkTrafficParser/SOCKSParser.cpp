//
// Created by vizhy on 05.11.20.

#include "SOCKSParser.h"


    bool SOCKSParser::IsClientInitiationMessage(const unsigned char *buffer, int32_t buffersize) {
        if ((sizeof(buffer)/sizeof(char)) == 0)
            return false;
        else {
            int32_t nMethods[buffer[1]];
            unsigned int Methods = buffer[1];
            if (buffer[2] == Proxy::Utilities::SOCKS5::Handshake::Version) {
                memcpy(&nMethods[0], &buffer[1], Methods);
            }
            if(Methods == 0x00)
                Methods = 1;
            if(Methods == 0xFE)
                Methods = 0xFE - 1;

            for (int i = 1; i < (Methods + 1); ++i) {
                switch (buffer[i]) {
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


