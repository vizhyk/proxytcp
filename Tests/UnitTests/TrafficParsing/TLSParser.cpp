#include <gtest/gtest.h>
#include <netinet/in.h>

#include "src/TrafficParsing/TLS/TLSParser.hpp"
#include "src/ByteStream/ByteStream.hpp"
#include "Tests/TestUtilities.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;
using namespace Proxy::Utilities;

TEST(TLSParsingTests, IsCLientHelloMessage)
{
    EXPECT_TRUE(TLSParser::IsClientHelloMessage(TLSValidClientHelloMessage, sizeof(TLSValidClientHelloMessage), Offsets::TLS::TLS_DATA));
    EXPECT_FALSE(TLSParser::IsClientHelloMessage(TLSInvalidClientHelloMessage, sizeof(TLSInvalidClientHelloMessage), Offsets::TLS::TLS_DATA));
}

TEST(TLSParsingTests, IsServerHelloMessage)
{
    EXPECT_TRUE(TLSParser::IsServerHelloMessage(TLSValidServerHelloMessage, sizeof(TLSValidServerHelloMessage), Offsets::TLS::TLS_DATA));
    EXPECT_FALSE(TLSParser::IsServerHelloMessage(TLSInvalidServerHelloMessage, sizeof(TLSInvalidServerHelloMessage), Offsets::TLS::TLS_DATA));
}


TEST(TLSParsingTests, ExtractUint32AndUint16FromNetworkData)
{
    const uint8_t validUint16NetworkData[] = {0x01, 0xbb};
    const uint8_t invalidUint16NetworkData[] = {0xbb};

    const uint8_t validUint32NetworkData[] = {0x01,0xbb,0x03,0x02};
    const uint8_t invalidUint32NetworkData[] = {0xbb,0x99};

    Proxy::Status status;
    EXPECT_TRUE(TLSParser::ExtractUint16FromNetworkData(validUint16NetworkData, sizeof(validUint16NetworkData), status) == 443 && status.Succeed());
    EXPECT_FALSE(TLSParser::ExtractUint16FromNetworkData(invalidUint16NetworkData, sizeof(invalidUint16NetworkData), status) == 443 && status.Failed());

    EXPECT_TRUE(TLSParser::ExtractUint32FromNetworkData(validUint32NetworkData, sizeof(validUint32NetworkData), status) == 0x01bb0302 && status.Succeed());
    EXPECT_FALSE(TLSParser::ExtractUint32FromNetworkData(invalidUint32NetworkData, sizeof(invalidUint32NetworkData), status) == 0x01bb0302 && status.Failed());

}

TEST(TLSParsingTests, GetDomainNameFromTLSPacket)
{
    Proxy::ByteStream domainName;
    auto status = TLSParser::GetDomainNameFromTLSPacket(TLSValidClientHelloMessage, sizeof(TLSValidClientHelloMessage), domainName);
    EXPECT_TRUE( status.Succeed());

    auto expectedDomainNameByteStream = Proxy::ByteStream(TLSExpectedDomainName, sizeof(TLSExpectedDomainName));

    EXPECT_TRUE( domainName == expectedDomainNameByteStream );
}


TEST(TLSParsingTests, GetRecordSize)
{
    auto recordSize = TLSParser::GetTLSRecordPayloadSize(TLSValidShortClientHelloMessage, sizeof(TLSValidShortClientHelloMessage));
    EXPECT_TRUE(recordSize == 0x09);

    recordSize = TLSParser::GetTLSRecordPayloadSize(TLSValidShortClientHelloMessage, 0);
    EXPECT_TRUE(recordSize == 0x00);

    recordSize = TLSParser::GetTLSRecordPayloadSize(TLSValidShortClientHelloMessage, 1);
    EXPECT_TRUE(recordSize == 0x00);
}

TEST(TLSParsingTests, GetMessageSize)
{
    auto messageSize = TLSParser::GetTLSMessageSize(TLSValidShortClientHelloMessage + Proxy::HeaderSize::TLS_RECORD + 1, sizeof(TLSValidShortClientHelloMessage));
    EXPECT_TRUE(messageSize == 0x7c);

    messageSize = TLSParser::GetTLSMessageSize(TLSValidShortClientHelloMessage + Proxy::HeaderSize::TLS_RECORD + 1, 0);
    EXPECT_TRUE(messageSize == 0x00);
}
