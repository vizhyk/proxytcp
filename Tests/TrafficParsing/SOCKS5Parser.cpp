#include <gtest/gtest.h>
#include "src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp"
#include "../TestUtilities.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;
using namespace Proxy::Utilities;

TEST(SOCKS5ParsingTests, IsValidClientHelloMessage)
{
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(socks5ValidClientHelloMessage, 3));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(socks5InvalidClientHelloMessage0, 3));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(socks5InvalidClientHelloMessage1, 2));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(socks5InvalidClientHelloMessage2, 0));
}


TEST(SOCKS5ParsingTests, GetAuthenticationMethod)
{
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(socks5ValidClientHelloMessage, 3));
    EXPECT_TRUE ( SOCKS5Parser::GetClientAuthenticationMethod(socks5ValidClientHelloMessage,3) == 0x00 );
}

TEST(SOCKS5ParsingTests, GetDestinationAddressAndPortByDomainName)
{
    uint8_t expectedAddress[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    Proxy::ByteStream tmpStream (expectedAddress, sizeof(expectedAddress));

    uint16_t port = 0;

    Proxy::ByteStream destinationAddress;
    auto status = SOCKS5Parser::GetDestinationAddressAndPort(socks5ConnectionRequestMessage, sizeof(socks5ConnectionRequestMessage),destinationAddress, port);

    EXPECT_TRUE( status.Succeed() && destinationAddress == tmpStream && port == Ports::HTTPS );
}

TEST(SOCKS5ParsingTests, ConnectionRequestParsingIncorrectBuffSize)
{
    Proxy::Status status;
    uint16_t port = 0;
    uint8_t expectedAddress[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    Proxy::ByteStream tmpStream (expectedAddress, sizeof(expectedAddress));
    Proxy::ByteStream destinationAddress;

    status = SOCKS5Parser::GetDestinationAddressAndPort(socks5ConnectionRequestMessage, 0,destinationAddress, port);

    EXPECT_TRUE(status.Failed());

    auto incorrectBuffSize = sizeof(expectedAddress) / 2;
    status = SOCKS5Parser::GetDestinationAddressAndPort(socks5ConnectionRequestMessage, incorrectBuffSize,destinationAddress, port);

    EXPECT_TRUE(status.Failed());
}
