#include <src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp>
#include <Connection/SocketConnection.hpp>
#include "ClientHelloTransmission.hpp"


namespace Proxy::SOCKS5Flow
{

    std::unique_ptr<ConversationFlow>
    ClientHelloTransmission::PerformTransaction(SocketConnection& clientConnection, SocketConnection& serverConnection, int32_t epollfd, int32_t sockfdWithEvent) noexcept
    {
//        std::cout << "SOCKS5Flow::ClientHello\n";
        using namespace  TrafficParsing;
        Status status;
        status = ReadAllDataFromConnection(clientConnection);
        if (status.Failed()) { return nullptr; }

        if (SOCKS5Parser::IsValidClientHelloMessage(clientConnection.Buffer().GetBuffer(),clientConnection.Buffer().GetUsedBytes()))
        {
           status = SendAllDataToConnection(GenerateClientInitiationResponse(), clientConnection);
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
        buffer.Insert(static_cast<uint8_t>(0x5));
        buffer.Insert (static_cast<uint8_t>(0x0));
        return  buffer;
    }

}
