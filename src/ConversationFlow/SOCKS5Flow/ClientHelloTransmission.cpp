#include <iostream>
#include <src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp>
#include "ClientHelloTransmission.hpp"


namespace Proxy::SOCKS5Flow
{

    std::unique_ptr<ConversationFlow>
    ClientHelloTransmission::PerformTransaction(ClientConnection& clientConnection, ServerConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
//        std::cout << "SOCKS5Flow::ClientHello\n";
        using namespace  TrafficParsing;
        Status status;
        status = ReadAllDataFromConnection(clientConnection);
        if (status.Failed()) { return nullptr; }

        if (SOCKS5Parser::IsValidClientHelloMessage(clientConnection.Buffer().GetBuffer(),clientConnection.Buffer().GetUsedBytes()))
        {
           status = SendAllDataToConnection(GenerateClientInitiationResponse(),clientConnection);
           if(status.Failed()) { return nullptr; }

           clientConnection.Buffer().Clear();

           return std::make_unique<ConnectionRequestTransmission>();
        }
        return nullptr;


    }

    ByteStream
    ClientHelloTransmission::GenerateClientInitiationResponse() noexcept
    {
        ByteStream buffer(2);
        buffer.Insert(static_cast<uint8_t>(0x05));
        buffer.Insert (static_cast<uint8_t>(0x00));
        return  buffer;
    }

}
