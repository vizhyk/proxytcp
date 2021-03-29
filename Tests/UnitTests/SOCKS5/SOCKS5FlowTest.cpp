#include <gtest/gtest.h>

#include "ByteStream/ByteStream.hpp"
#include "Utilities/SOCKS5.hpp"
#include "Tests/TestUtilities.hpp"
#include "ConversationFlow/SOCKS5Flow/ClientHelloTransmission.hpp"
#include "ConversationManager/SocketConversationManager.hpp"

namespace Proxy::SOCKS5Flow
{

    TEST(SOCKS5Test, BadMakeSocketNonblocking)
    {
        Status status;

        status = ConnectionRequestTransmission::MakeSocketNonblocking(-1);
        EXPECT_EQ(status, Status::Error::BadMakingSocketNonblocking);
    }

    TEST(SOCKS5Test, BadCreateSocketForForwardingByHostname)
    {
        Status status;
        int32_t destport = 1;
        int32_t destport2 = 2;
        int32_t epollfd = 1;
        int32_t epollfd2 = 2;
        const uint8_t validHostname[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61, 0x00};
        const uint8_t invalidHostname[] = {0x6e};

        auto result = ConnectionRequestTransmission::CreateSocketForForwardingByHostname(status, destport, invalidHostname, epollfd);
        EXPECT_EQ(result,-1);
        EXPECT_EQ(status, Status::Error::BadGetAddrInfo);

        result = ConnectionRequestTransmission::CreateSocketForForwardingByHostname(status, destport2, validHostname, epollfd2);
        EXPECT_EQ(result,-1);
        EXPECT_EQ(status, Status::Error::BadEpollCTL);
    }

    TEST(SOCKS5Test, BadTryConnectToServer)
    {
        Status status;
        int32_t serverSockfd = 2;
        const uint8_t hostname[] = {0x6e, 0x6f, 0x76, 0x61, 0x2e, 0x6e, 0x65, 0x74, 0x2e, 0x75, 0x61};

        SocketConnection connection(0, Connection::ConnectionSide::Server, nullptr);

        status = ConnectionRequestTransmission::TryConnectToTheServer(connection,hostname,80,Utilities::SOCKS5::Handshake::IPv4,0,serverSockfd);
        EXPECT_EQ(status, Status::Error::BadConnectToServer);

        status = ConnectionRequestTransmission::TryConnectToTheServer(connection,hostname,80,Utilities::SOCKS5::Handshake::DomainName,0,serverSockfd);
        EXPECT_EQ(status, Status::Error::BadGetAddrInfo);

        status = ConnectionRequestTransmission::TryConnectToTheServer(connection,hostname,80,9,0,serverSockfd);
        EXPECT_EQ(status, Status::Error::BadDestinationAddressType);
    }

    TEST(SOCKS5Test, BadGenerateConnectionRequestReply)
    {
        Status status;
        ByteStream stream;
        auto sizeOf = sizeof(TestUtilities::SOCKS5ConnectionRequestMessage2);

        status = ConnectionRequestTransmission::GenerateConnectionRequestReply(TestUtilities::SOCKS5ConnectionRequestMessage2,sizeOf, stream);
        EXPECT_EQ(status, Status::Error::BadConnectionRequestLenght);
    }

    TEST(SOCKS5Test, BadIsConnectionSucceed)
    {
        Status status;

        status = ConnectionRequestTransmission::IsConnectionSucceed(-12);
        EXPECT_EQ(status, Status::Error::BadGetSockopt);

    }


};




