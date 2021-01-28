#include <gtest/gtest.h>
#include "ByteStream/ByteStream.hpp"
#include "Utilities/SOCKS5.hpp"
#include "ConversationFlow/SOCKS5Flow/ClientHelloTransmission.hpp"
#include "ConversationManager/ConversationManager.hpp"



namespace Proxy::SOCKS5Flow
{
    class SOCKS5Test : public ::testing::Test
    {
    public:
        ConversationManager object;

    };

TEST_F(SOCKS5Test, Generate)
{
    Proxy::ByteStream buffer(2);
    buffer = ClientHelloTransmission::GenerateClientInitiationResponse();
    EXPECT_TRUE(buffer.GetUsedBytes() == 2);
    EXPECT_EQ(buffer[0], 0x05);
    EXPECT_EQ(buffer[1], 0x00);
}

TEST_F(SOCKS5Test, MakeSocketNonblocking)
{
    int32_t socket = 1;
    EXPECT_EQ(ConnectionRequestTransmission::MakeSocketNonblocking(socket), Status::Success::Success);
}

TEST_F(SOCKS5Test, CreateSocketForForwardingByHostname)
{
    Status report;
    int32_t destport = 1;
    int32_t epollfd = 1;
    const uint8_t hostname[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    EXPECT_EQ(
            ConnectionRequestTransmission::CreateSocketForForwardingByHostname(report, destport, hostname, epollfd),
            -1);
}

TEST_F(SOCKS5Test, CreateSocketForForwardingByIP)
{
    int32_t socket = 1;
    int32_t destport = 1;
    uint8_t addr[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    EXPECT_EQ(ConnectionRequestTransmission::CreateSocketForForwardingByIP(socket, destport, addr),
              Status::Error::BadDestinationHost);
}

TEST_F(SOCKS5Test,GenerateConnectionRequestReply)
{
    const uint8_t buffer[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};
    size_t buffersize = sizeof(buffer);
    ByteStream reply;
    EXPECT_EQ(ConnectionRequestTransmission::GenerateConnectionRequestReply(buffer,buffersize,reply), Status::Error::BadConnectionRequestLenght );
}

TEST_F(SOCKS5Test,IsConnectionSucceed)
{
    int32_t  sockfd = 1;
    EXPECT_EQ(ConnectionRequestTransmission::IsConnectionSucceed(sockfd), Status::Error::BadGetSockopt);
}

TEST_F(SOCKS5Test,RemoveUnactivePipeline)
{
    int32_t  sockfd = 1;
    EXPECT_EQ(object.ConversationManager::RemoveUnactivePipeline(sockfd), Status::Error::NoConversationPipelineFound);
}

};




