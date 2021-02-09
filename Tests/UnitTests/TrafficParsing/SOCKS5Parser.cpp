#include <gtest/gtest.h>
#include "src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp"
#include "Tests/TestUtilities.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;
using namespace Proxy::Utilities;

TEST(SOCKS5ParsingTests, IsValidClientHelloMessage)
{
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage, sizeof(SOCKS5ValidClientHelloMessage)));
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage1, sizeof(SOCKS5ValidClientHelloMessage1)));
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage2, sizeof(SOCKS5ValidClientHelloMessage2)));
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage3, sizeof(SOCKS5ValidClientHelloMessage3)));
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage4, sizeof(SOCKS5ValidClientHelloMessage4)));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage0, 3));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage1, 2));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage2, 0));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage3, 3));
}


TEST(SOCKS5ParsingTests, GetAuthenticationMethod)
{
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage, 3));
    EXPECT_TRUE (SOCKS5Parser::GetClientAuthenticationMethod(SOCKS5ValidClientHelloMessage, 3) == 0x00 );
}

TEST(SOCKS5ParsingTests, GetDestinationAddressAndPortByDomainName)
{
    uint8_t expectedAddress[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    Proxy::ByteStream tmpStream (expectedAddress, sizeof(expectedAddress));

    uint16_t port = 0;

    Proxy::ByteStream destinationAddress;
    auto status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage, sizeof(SOCKS5ConnectionRequestMessage), destinationAddress, port);

    EXPECT_TRUE( status.Succeed() && destinationAddress == tmpStream && port == Ports::HTTPS );
}

TEST(SOCKS5ParsingTests, GetDestinationAddressAndPortByIPv4_IPv6)
{
    uint16_t port = 0;
    Proxy::Status status;
    Proxy::ByteStream destinationAddress;

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage3, sizeof(SOCKS5ConnectionRequestMessage3), destinationAddress, port);
    EXPECT_TRUE( status.Succeed());

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage3, sizeof(SOCKS5ConnectionRequestMessage3) - 3, destinationAddress, port);
    EXPECT_TRUE( status == Proxy::Status::Error::BadBufferSize);

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage4, sizeof(SOCKS5ConnectionRequestMessage4), destinationAddress, port);
    EXPECT_TRUE( status.Succeed());

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage4, sizeof(SOCKS5ConnectionRequestMessage4) - 3, destinationAddress, port);
    EXPECT_TRUE( status == Proxy::Status::Error::BadBufferSize);

}

TEST(SOCKS5ParsingTests, ConnectionRequestParsingIncorrectBuffSize)
{
    Proxy::Status status;
    uint16_t port = 0;
    uint8_t expectedAddress[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    Proxy::ByteStream tmpStream (expectedAddress, sizeof(expectedAddress));
    Proxy::ByteStream destinationAddress;

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage, 0, destinationAddress, port);
    EXPECT_TRUE(status.Failed());

    auto incorrectBuffSize = sizeof(expectedAddress) / 2;

    status = SOCKS5Parser::GetDestinationAddressAndPort(SOCKS5ConnectionRequestMessage, incorrectBuffSize, destinationAddress, port);
    EXPECT_TRUE(status.Failed());
}

TEST(SOCKS5ParsingTests, IsValidConnectionRequestMessage)
{
    EXPECT_TRUE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5ConnectionRequestMessage, sizeof(SOCKS5ConnectionRequestMessage)));
    EXPECT_TRUE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5ConnectionRequestMessage0, sizeof(SOCKS5ConnectionRequestMessage0)));
    EXPECT_TRUE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5ConnectionRequestMessage1, sizeof(SOCKS5ConnectionRequestMessage1)));
    EXPECT_FALSE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5InvalidConnectionRequestMessage, sizeof(SOCKS5InvalidConnectionRequestMessage)));
    EXPECT_FALSE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5InvalidConnectionRequestMessage0, sizeof(SOCKS5InvalidConnectionRequestMessage0)));
    EXPECT_FALSE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5InvalidConnectionRequestMessage1, sizeof(SOCKS5InvalidConnectionRequestMessage1)));
    EXPECT_FALSE(SOCKS5Parser::IsValidConnectionRequestMessage(SOCKS5InvalidConnectionRequestMessage2, sizeof(SOCKS5InvalidConnectionRequestMessage2)));

}

TEST(SOCKS5ParsingTests,GetConnectionRequestLength)
{
    uint32_t length;
    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage, sizeof(SOCKS5ConnectionRequestMessage));
    EXPECT_EQ(length, sizeof(SOCKS5ConnectionRequestMessage));

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage0, sizeof(SOCKS5ConnectionRequestMessage0));
    EXPECT_EQ(length, sizeof(SOCKS5ConnectionRequestMessage0));

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage1, sizeof(SOCKS5ConnectionRequestMessage1));
    EXPECT_EQ(length, sizeof(SOCKS5ConnectionRequestMessage1));

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage, sizeof(SOCKS5InvalidConnectionRequestMessage));
    EXPECT_EQ(length, -1);

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage0, sizeof(SOCKS5InvalidConnectionRequestMessage0));
    EXPECT_EQ(length, -1);

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage1, sizeof(SOCKS5InvalidConnectionRequestMessage1));
    EXPECT_EQ(length, -1);

    length = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage2, sizeof(SOCKS5InvalidConnectionRequestMessage2));
    EXPECT_EQ(length, -1);
}
