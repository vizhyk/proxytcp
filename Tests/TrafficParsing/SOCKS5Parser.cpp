#include <gtest/gtest.h>
#include "src/TrafficParsing/SOCKS5/SOCKS5Parser.hpp"
#include "../TestUtilities.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;
using namespace Proxy::Utilities;

TEST(SOCKS5ParsingTests, IsValidClientHelloMessage)
{
    EXPECT_TRUE (SOCKS5Parser::IsValidClientHelloMessage(SOCKS5ValidClientHelloMessage, 3));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage0, 3));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage1, 2));
    EXPECT_FALSE(SOCKS5Parser::IsValidClientHelloMessage(SOCKS5InvalidClientHelloMessage2, 0));
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

}

TEST(SOCKS5ParsingTests,GetConnectionRequestLength)
{
    size_t  test_value0, test_value1, test_value2, test_value3, test_value4, test_value5;
    test_value0 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage, sizeof(SOCKS5ConnectionRequestMessage));
    test_value1 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage0, sizeof(SOCKS5ConnectionRequestMessage0));
    test_value2 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5ConnectionRequestMessage1, sizeof(SOCKS5ConnectionRequestMessage1));
    test_value3 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage, sizeof(SOCKS5InvalidConnectionRequestMessage));
    test_value4 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage0, sizeof(SOCKS5InvalidConnectionRequestMessage0));
    test_value5 = SOCKS5Parser::GetConnectionRequestLength(SOCKS5InvalidConnectionRequestMessage1, sizeof(SOCKS5InvalidConnectionRequestMessage1));
    EXPECT_EQ(test_value0, sizeof(SOCKS5ConnectionRequestMessage));
    EXPECT_EQ(test_value1, sizeof(SOCKS5ConnectionRequestMessage0));
    EXPECT_EQ(test_value2, sizeof(SOCKS5ConnectionRequestMessage1));
    EXPECT_EQ(test_value3, -1);
    EXPECT_EQ(test_value4, -1);
    EXPECT_EQ(test_value5, -1);

}
