#include <gtest/gtest.h>
#include "Connection/SocketCapturingConnection.hpp"
#include "Status.hpp"

namespace Proxy
{
    TEST(ConnectionTest, ConnectionEquality)
    {
        SocketConnection s(0,Connection::ConnectionSide::Client, nullptr);
        SocketConnection s1(0,Connection::ConnectionSide::Client, nullptr);
        SocketConnection s2(1,Connection::ConnectionSide::Client, nullptr);

        EXPECT_EQ(s,s1);
        EXPECT_TRUE(s2 != s1);
    }


    TEST(ConnectionTest, SocketConnectionSendDataTo)
    {
        SocketConnection s1(0,Connection::ConnectionSide::Client, nullptr);
        SocketConnection s2(1,Connection::ConnectionSide::Client, nullptr);
        SocketCapturingConnection s3(2,Connection::ConnectionSide::Client, nullptr);
        SocketCapturingConnection s4(3,Connection::ConnectionSide::Client, nullptr);

        ByteStream tmp(10);

        Status status = s2.SendDataTo(tmp,s1);
        EXPECT_TRUE(status == Status::Error::BadSendingData);

        status = s3.SendDataTo(tmp,s4);
        EXPECT_TRUE(status == Status::Error::BadSendingData);
    }
}