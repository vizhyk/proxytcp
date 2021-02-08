#include <gtest/gtest.h>

#include "src/TrafficParsing/TLS/TLSRecordParser.hpp"
#include "src/ByteStream/ByteStream.hpp"
#include "Tests/TestUtilities.hpp"
#include "Status.hpp"

using namespace TestUtilities;
using namespace Proxy::TrafficParsing;
using namespace Proxy::Utilities;
using namespace Proxy;

TEST(TLSRecordParserTests, GetTLSRecordPayloadSize)
{
    uint8_t invalidRecord[3] = {0x01,0x03,0x03};
    auto result = TLSRecordParser::GetTLSRecordPayloadSize(invalidRecord,sizeof(invalidRecord));

    EXPECT_EQ(result,0);
}

TEST(TLSRecordParserTests, IsRecordFull)
{
    Status status;

    status = TLSRecordParser::IsRecordFull(TLSInvalidClientHelloMessage, 0);

    EXPECT_EQ(status, Status::Success::WaitingForData);
}

