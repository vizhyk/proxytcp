#include <gtest/gtest.h>

#include "src/TrafficParsing/TLS/TLSParser.hpp"
#include "src/ByteStream/ByteStream.hpp"
#include "../TestUtilities.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;

TEST(TLSParsingTests, IsCLientHelloMessage)
{
    EXPECT_TRUE( TLSParser::IsClientHelloMessage(TLSValidClientHelloMessage, 0) );
    EXPECT_FALSE(TLSParser::IsClientHelloMessage(TLSInvalidClienHelloMessage, 0) );
}


TEST(TLSParsingTests, GetDomainNameFromTCPPacket)
{
    auto domainName = TLSParser::GetDomainNameFromTCPPacket(TLSValidClientHelloMessage, 0);
    auto expectedDomainNameByteStream = Proxy::ByteStream(TLSExpectedDomainName, sizeof(TLSExpectedDomainName));

    EXPECT_TRUE( domainName == expectedDomainNameByteStream );
}

